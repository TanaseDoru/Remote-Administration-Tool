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

    clientHndler.numberClients=0;

    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    sock_init(&socket_desc, &server);

    pthread_t tid;
    if (pthread_create(&tid, NULL, serverInterface, NULL) < 0) {
        perror("Could not create interface thread");
        
    }
    while (1) {
        parameters_t *params = malloc(sizeof(parameters_t));
        if (params == NULL) {
            perror("Failed to allocate memory for thread parameters");
            continue;
        }

        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_sock < 0) {
            perror("Accept failed");
            free(params);
            continue;
        }

        //printf("Connection accepted for %d\n", client_sock);

        ////De aici introducem in vector un nou socket de client

        clientHndler.socketsClients[clientHndler.numberClients++] = client_sock;

        // Initialize client attributes
        setZeroClientHandler(&clientHndler.clientsAttr[client_sock]);

        params->client_sock = client_sock;
        params->socket_desc = socket_desc;

        if (pthread_create(&tid, NULL, handle_client, params) < 0) {
            perror("Could not create thread");
            close(client_sock);
            free(params);
            continue;
        }

        pthread_detach(tid);
    }
    return 0;
}
