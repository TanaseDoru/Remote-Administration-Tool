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

int main() {

    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    sock_init(&socket_desc, &server);
    pthread_t tid;

    while(1)
    {
        pthread_t tid;
        if (pthread_create(&tid, NULL, serverInterface, NULL) < 0) {
            perror("Could not create thread");
            continue;
        }
        

        parameters_t params;
        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_sock < 0) {
            perror("Accept failed");
            return 1;
        }
        printf("Connection accepted for %d\n", client_sock);
        
        ///Initializare cu valori default pentru clientAttr
        setZeroClientHandler(&clientHndler.clientsAttr[client_sock]);

        params.client_sock = client_sock;
        params.socket_desc = socket_desc;
        if (pthread_create(&tid, NULL, handle_client, &params) < 0) {
            perror("Could not create thread");
            continue;
        }


        pthread_detach(tid);
    }
    return 0;
}
