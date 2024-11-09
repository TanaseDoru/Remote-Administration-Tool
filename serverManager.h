#pragma once
#include <arpa/inet.h>

void* handle_client(void* params);

void sock_init(int *socket_desc, struct sockaddr_in* server);
