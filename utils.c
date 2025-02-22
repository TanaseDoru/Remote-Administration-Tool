#include "utils.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

int sendMessage(int sock, message_t *msg)
{
    uint32_t total_size = sizeof(msg->opCode) + sizeof(msg->size) + msg->size;

    if (send(sock, msg, total_size, 0) == -1)
    {
        perror("Failed to send message");
        return -1;
    }
    return 0;
}

int recvMessage(int sock, message_t *msg)
{
    int readSize;
    if ((readSize = recv(sock, msg, sizeof(*msg), 0)) > 0)
    {
        msg->buffer[msg->size] = '\0';
    }
    return readSize;
}

void sendFile(int sock, const char *filename)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_sent;
    // printf("In function:\n");
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        // printf("Sending ||%s||:%d:\n", buffer, (int)bytes_read);
        bytes_sent = send(sock, buffer, bytes_read, 0);
        if (bytes_sent < 0)
        {
            perror("Failed to send data");
            close(file_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read < 0)
    {
        perror("Failed to read file");
    }
    sleep(1);
    // printf("Sending Stop:\n");
    fflush(0);
    strcpy(buffer, "STOP");
    send(sock, buffer, sizeof("STOP"), 0);
    close(file_fd);
}

void recvFile(int sock, const char *filename)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received, bytes_written;
    int fileFd = open(filename, O_CREAT | O_WRONLY, 0666);

    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        // printf("||%s||:%lu:\n", buffer, bytes_received);
        // fflush(0);
        if (buffer[bytes_received - 2] == 'P' && buffer[bytes_received - 3] == 'O')
        {
            // printf("recieved Stop: %s\n", buffer);
            // fflush(0);
            break;
        }
        bytes_written = write(fileFd, buffer, bytes_received);
        if (bytes_written < 0)
        {
            perror("Failed to write to file");
            close(fileFd);
            return;
        }
    }

    if (bytes_received < 0)
    {
        perror("Failed to receive data");
    }
    else
    {
        // printf("Client connected successfully. Press a key to continue.\n");
    }

    close(fileFd);
}

void initTaskQueue(task_queue_t *queue)
{
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

void enqueueTask(task_queue_t *queue, task_t task)
{
    pthread_mutex_lock(&queue->mutex);

    while (queue->count == MAX_TASKS)
    {
        
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    queue->rear = (queue->rear + 1) % MAX_TASKS;
    queue->tasks[queue->rear] = task;
    queue->count++;

    
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

task_t dequeueTask(task_queue_t *queue)
{
    pthread_mutex_lock(&queue->mutex);

    while (queue->count == 0)
    {
        
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    task_t task = queue->tasks[queue->front];
    queue->front = (queue->front + 1) % MAX_TASKS;
    queue->count--;

   
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    return task;
}

void destroyTaskQueue(task_queue_t *queue)
{
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
}
