#include "utils.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void send_message(int sock, message_t *msg) {
    uint32_t total_size = sizeof(msg->opCode) + sizeof(msg->size) + msg->size;

    if (send(sock, msg, total_size, 0) == -1) {
        perror("Failed to send message");
    }
}