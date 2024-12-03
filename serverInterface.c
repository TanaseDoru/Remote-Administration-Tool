#include "serverInterface.h"
#include "constraints.h"
#include "serverManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

extern clientHandler_t clientHndler;


void initTerminal() {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &new_tio);
    new_tio.c_lflag &= ~(ICANON | ECHO);  // Non-canonical mode, no echo
    new_tio.c_cc[VMIN] = 0;              // Non-blocking input
    new_tio.c_cc[VTIME] = 1;             // 0.1 second timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void resetTerminal() {
    struct termios default_tio;
    tcgetattr(STDIN_FILENO, &default_tio);
    default_tio.c_lflag |= (ICANON | ECHO);  // Restore canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &default_tio);
}   


void printConectedDevices(int* poz){
    int nrDeviceuri=0;
    char nume_deviceuri[MAX_CLIENTS][BUFFER_SIZE];

    for(int i = 0; i < clientHndler.numberClients; i++)
    {
        strcpy(nume_deviceuri[nrDeviceuri++], clientHndler.clientsAttr[clientHndler.socketsClients[i]].name);
    }

    if(*poz>=nrDeviceuri && *poz > 0)
        *poz=nrDeviceuri - 1;


    printf("\n\n\tDevices: \n");
    for(int i = 0; i<nrDeviceuri; i++){
        if(*poz==i)
            printf("\t\t  %s <--\n", nume_deviceuri[i]);
        else
            printf("\t\t%s \n", nume_deviceuri[i]);
    }
    
}

void* serverInterface()
{
    int poz =0;
    while(1){
        system("clear");
        printf("\t██████╗  █████╗ ████████╗   ████████╗██       ██╗\n");
        printf("\t██╔══██╗██╔══██╗ ╚═██╔══╝    ╚═██╔══╝╚██     ██╔╝\n");
        printf("\t██████╔╝███████║   ██║   ███   ██║    ╚██   ██╔╝\n");
        printf("\t██╔══██╗██╔══██║   ██║         ██║     ╚██ ██╔╝   \n");
        printf("\t██║  ██║██║  ██║   ██║         ██║      ╚██╔╝   \n");
        printf("\t╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝         ╚═╝       ╚═╝   \n");
        printConectedDevices(&poz);
        char ch = '\0';
        initTerminal();
        ch = getchar();  // Non-blocking getchar
        resetTerminal();
            
            if (ch == 'w') {
                if(poz!=0)
                    poz--;
            } else if (ch == 's') {
                poz++;
            }

        sleep(1);
    }
}