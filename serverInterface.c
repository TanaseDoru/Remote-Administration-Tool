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
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echoing
    term.c_cc[VMIN] = 1;  // Read one character at a time
    term.c_cc[VTIME] = 0; // No timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &term); // Apply changes
}

void resetTerminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);  // Enable canonical mode and echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &term); // Apply changes
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
        char ch;
        initTerminal();  // Set terminal to non-canonical mode

            // Consume the '['
            getchar();
            getchar();
            ch = getchar();  // Get the arrow key character
            
            if (ch == 'A') {
                if(poz!=0)
                    poz--;
            } else if (ch == 'B') {
                poz++;
            }


        resetTerminal();  // Reset terminal to normal mode
    }
}