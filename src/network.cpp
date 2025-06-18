#include "network.hpp"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

bool init_networking() {
#ifdef _WIN32
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2,2), &wsa) == 0;
#else
    return true;
#endif
}

void shutdown_networking() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void close_socket(socket_t sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

socket_t connect_to_server(const char* ip, int port) {
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed\n";
        close_socket(sock);
        return -1;
    }

    return sock;
}

int send_message(socket_t sock, const char* msg) {
    return send(sock, msg, strlen(msg), 0);
}

int receive_message(socket_t sock, char* buffer, int length) {
    int bytes = recv(sock, buffer, length - 1, 0);
    if (bytes >= 0) buffer[bytes] = '\0';
    return bytes;
}
