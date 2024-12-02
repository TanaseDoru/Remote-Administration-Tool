#pragma once
#include <sys/socket.h>
#include "messageManager.h"


struct clientInfo
{
    char name[BUFFER_SIZE];
    int serverSocket;
};

typedef struct clientInfo clientInfo_t;

void connectionInitialize(int* sock);

void startingDataInitialize();

void handleOpcode(int sock, message_t msg);