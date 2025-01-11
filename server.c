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
task_queue_t taskQueue; // Coada de task-uri

#define THREAD_POOL_SIZE 4

// Funcție pentru inițializarea thread pool-ului
void initThreadPool(pthread_t *threadPool, int poolSize)
{
    for (int i = 0; i < poolSize; i++)
    {
        if (pthread_create(&threadPool[i], NULL, handle_client, NULL) < 0)
        {
            perror("Failed to create thread in pool");
            exit(EXIT_FAILURE);
        }
        pthread_detach(threadPool[i]); // Detach pentru a evita join-ul manual
    }
}

int main()
{
    clientHndler.numberClients = 0;

    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    // Inițializare socket server
    sock_init(&socket_desc, &server);

    // Inițializare coadă de task-uri
    initTaskQueue(&taskQueue);

    // Inițializare thread pool
    pthread_t threadPool[THREAD_POOL_SIZE];
    initThreadPool(threadPool, THREAD_POOL_SIZE);

    // Pornire interfață server (thread separat pentru funcționalități suplimentare)
    pthread_t tid;
    if (pthread_create(&tid, NULL, serverInterface, NULL) < 0)
    {
        perror("Could not create interface thread");
    }
    pthread_detach(tid);

    // Acceptarea conexiunilor de la clienți
    while (1)
    {
        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_sock < 0)
        {
            perror("Accept failed");
            continue;
        }

        // Adăugăm socket-ul clientului în lista de clienți activi
        clientHndler.socketsClients[clientHndler.numberClients++] = client_sock;

        // Inițializăm atributele clientului
        setZeroClientHandler(&clientHndler.clientsAttr[client_sock]);

        // Creăm un task pentru client și îl adăugăm în coadă
        task_t newTask;
        newTask.client_sock = client_sock;
        enqueueTask(&taskQueue, newTask);
    }

    return 0;
}
