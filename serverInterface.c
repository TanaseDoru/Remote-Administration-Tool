#include "serverInterface.h"
#include "constraints.h"
#include "serverManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/select.h>

extern clientHandler_t clientHndler;

void initTerminal()
{
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= ~(ICANON | ECHO);
    new_tio.c_cc[VMIN] = 1;  // Read at least 1 character
    new_tio.c_cc[VTIME] = 0; // No timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void resetTerminal()
{
    struct termios default_tio;
    tcgetattr(STDIN_FILENO, &default_tio);
    default_tio.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &default_tio);
}

void printConectedDevices(int *poz)
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
}

void *serverInterface()
{
    int poz = 0;
    fd_set read_fds;
    struct timeval timeout;

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
        printConectedDevices(&poz);

        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int ready = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, NULL);

        if (ready > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
        {
            char ch = getchar();
            ch = getchar();
            ch = getchar();
            if (ch == 'A')
            {
                if (poz > 0)
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
