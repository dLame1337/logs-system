#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

namespace network {
    class tcp_server {
    public:
        tcp_server(unsigned short port);
        ~tcp_server();
        bool listen(int backlog = 5);
        SOCKET accept();
        void close();

    private:
        unsigned short port_;
        SOCKET listen_fd_;
    };
}