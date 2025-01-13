#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "serverInterface.h"
#include "utils.h"
#include "serverManager.h"

clientHandler_t clientHndler;
task_queue_t taskQueue; 

#define THREAD_POOL_SIZE 4


void initThreadPool(pthread_t *threadPool, int poolSize)
{
    for (int i = 0; i < poolSize; i++)
    {
        if (pthread_create(&threadPool[i], NULL, handle_client, NULL) < 0)
        {
            perror("Failed to create thread in pool");
            exit(EXIT_FAILURE);
        }
        pthread_detach(threadPool[i]); 
    }
}

int main()
{
    clientHndler.numberClients = 0;

    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    
    sock_init(&socket_desc, &server);

    
    initTaskQueue(&taskQueue);

    
    pthread_t threadPool[THREAD_POOL_SIZE];
    initThreadPool(threadPool, THREAD_POOL_SIZE);

    
    pthread_t tid;
    if (pthread_create(&tid, NULL, serverInterface, NULL) < 0)
    {
        perror("Could not create interface thread");
    }
    pthread_detach(tid);

    
    while (1)
    {
        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_sock < 0)
        {
            perror("Accept failed");
            continue;
        }

        
        clientHndler.socketsClients[clientHndler.numberClients++] = client_sock;

        
        setZeroClientHandler(&clientHndler.clientsAttr[client_sock]);

        
        task_t newTask;
        newTask.client_sock = client_sock;
        enqueueTask(&taskQueue, newTask);
    }

    return 0;
}
