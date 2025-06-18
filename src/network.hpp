#pragma once

#ifdef _WIN32
    typedef unsigned long socket_t; // Same as SOCKET
#else
    typedef int socket_t;
#endif

bool init_networking();
void shutdown_networking();
socket_t connect_to_server(const char* ip, int port);
void close_socket(socket_t sock);
int send_message(socket_t sock, const char* msg);
int receive_message(socket_t sock, char* buffer, int length);
