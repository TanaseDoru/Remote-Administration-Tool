#include "serverInterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

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
    int nrDeviceuri=3;
    char nume_deviceuri[4][10]={"Device1", "Device2", "Device3"};

    // if(*poz<0)
    //     poz=0;
    if(*poz>=nrDeviceuri)
        *poz=nrDeviceuri-1;

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
    int poz =1;
    while(1){
        system("clear");
        printf("\t██████╗ █████╗ █████████╗   ████████╗██       ██╗\n");
        printf("\t██╔══██╗██╔══██╗╚══██╔══╝   ╚══██╔══╝╚██     ██╔╝\n");
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
            } else {
                printf("Other key pressed -> EXITING %c\n", ch);
                resetTerminal();
                break;
            }

        resetTerminal();  // Reset terminal to normal mode
    }
}