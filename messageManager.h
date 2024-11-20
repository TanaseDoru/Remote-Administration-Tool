#pragma once

#include "constraints.h"

struct message
{
    int size;
    char opCode;
    char buffer[SEND_BUFFER_SIZE];
};


typedef struct message message_t;

void encapsulateMessage(message_t *msg, char buffer[SEND_BUFFER_SIZE], char opCode);
void decapsulateMessage(message_t msg, char buffer[SEND_BUFFER_SIZE], char *opCode);
