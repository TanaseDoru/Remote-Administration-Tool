Site-urile se blocheaza introducandu-le in fisierul `/etc/hosts` ca formatul
```
127.0.0.1 www.site.com
```

```c
void handleIpOpcode(message_t msg)

{

FILE *hosts_file = fopen(HOSTS_FILE, "r"); // Open /etc/hosts in read mode

if (hosts_file == NULL)

{

perror("Error opening /etc/hosts");

return;

}

  

// Create a temporary file to store the updated hosts content

FILE *temp_file = fopen("/tmp/hosts.tmp", "w");

if (temp_file == NULL)

{

perror("Error opening temporary file");

fclose(hosts_file);

return;

}

  

char line[512];

// Read /etc/hosts line by line

while (fgets(line, sizeof(line), hosts_file))

{

if (strstr(line, "www.") == NULL)

{

// If not, write the line to the temporary file

fputs(line, temp_file);

}

}

  

fclose(hosts_file);

  

char *site = strtok(msg.buffer, "\n");

while (site != NULL)

{

printf("Blocking: %s\n", site);

// Append each site to the temporary file redirecting to 127.0.0.1

fprintf(temp_file, "127.0.0.1 %s\n", site);

site = strtok(NULL, "\n"); // Get the next site

}

  

fclose(temp_file);

  

if (rename("/tmp/hosts.tmp", HOSTS_FILE) == 0)

{

//printf("Updated /etc/hosts successfully.\n");

}

else

{

perror("Error replacing /etc/hosts");

}

}
```