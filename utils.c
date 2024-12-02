#include "utils.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int sendMessage(int sock, message_t *msg) {
    uint32_t total_size = sizeof(msg->opCode) + sizeof(msg->size) + msg->size;

    if (send(sock, msg, total_size, 0) == -1) {
        perror("Failed to send message");
        return -1;
    }
    return 0;
}


int recvMessage(int sock, message_t *msg)
{
    int readSize;
    if((readSize = recv(sock, msg, sizeof(*msg), 0)) > 0) {
        msg->buffer[msg->size] = '\0';

    }
    return readSize;
}