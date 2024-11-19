#include "messageManager.h"
#include <string.h>

void encapsulateMessage(message_t *msg, char* buffer[SEND_BUFFER_SIZE], char opCode)
{
    msg->opCode = opCode;
    strcpy(msg->buffer, buffer);
    msg->size = strlen(buffer);
}

void decapsulateMessage(message_t msg, char* buffer[SEND_BUFFER_SIZE], char *opCode)
{
    msg.buffer[msg.size] = '\0';
    strcpy(buffer, msg.buffer);
    *opCode = msg.opCode;
}