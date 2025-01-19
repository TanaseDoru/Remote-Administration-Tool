Folosind fork, se opreste procesul parinte, si ramane doar procesul copil care este adoptat de init

```c
pid_t pid, sid;

  

pid = fork();

  

if (pid < 0)

{

perror("Fork failed");

exit(1);

}

  

if (pid > 0)

{

printf("Parent process exiting, child will be adopted by init\n");

exit(0);

}

  

sid = setsid();

if (sid < 0)

{

perror("Failed to create a new session");

exit(1);

}

  

if (chdir("/") < 0)

{

perror("Failed to change working directory to /");

exit(1);

}

  

close(STDIN_FILENO);

close(STDOUT_FILENO);

close(STDERR_FILENO);

  

open("/dev/null", O_RDONLY);

open("/dev/null", O_WRONLY);

open("/dev/null", O_WRONLY);

  

printf("This process is now detached and running as a daemon.\n");
```