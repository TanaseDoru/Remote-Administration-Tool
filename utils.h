#pragma once
#include "messageManager.h"
#include "constraints.h"
#include <pthread.h>

struct parameters
{
    int socket_desc;
    int client_sock;
};

typedef struct parameters parameters_t;

struct keyLoggerParameters
{
    char keyboardFile[BUFFER_SIZE];
    int sock;
};

// Structura unui task
typedef struct
{
    int client_sock;
} task_t;

// Structura cozii de task-uri
typedef struct
{
    task_t tasks[MAX_TASKS];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} task_queue_t;

typedef struct keyLoggerParameters keyLoggerParameters_t;

int sendMessage(int sock, message_t *msg);

int recvMessage(int sock, message_t *msg);

void recvFile(int sock, const char *filename);

void sendFile(int sock, const char *filename);

void initTaskQueue(task_queue_t *queue);

void enqueueTask(task_queue_t *queue, task_t task);

task_t dequeueTask(task_queue_t *queue);
void destroyTaskQueue(task_queue_t *queue);