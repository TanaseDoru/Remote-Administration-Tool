#pragma once
#include "messageManager.h"

struct parameters
{
    int socket_desc;
    int client_sock;
};



typedef struct parameters parameters_t;

void send_message(int sock, message_t *msg);