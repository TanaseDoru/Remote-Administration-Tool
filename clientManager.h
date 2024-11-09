#pragma once

#define NAME_SIZE 100


struct clientAttributes
{
    int recordKeys;
    char* name[NAME_SIZE];
    int keylogger_fd;
};

typedef struct clientAttributes clientAttributes_t;