#include <iostream>
#include "network/tcp.h"
#include "log.h"
namespace network {
    tcp_server::tcp_server(unsigned short port) : port_(port), listen_fd_(INVALID_SOCKET) {}

    tcp_server::~tcp_server() {
        if (listen_fd_ != INVALID_SOCKET) {
            close();
        }
    }

    bool tcp_server::listen(int backlog) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != NO_ERROR) {
            LOGE("WSAStartup failed with error: " << result);
            return false;
        }

        listen_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listen_fd_ == INVALID_SOCKET) {
            LOGE("socket creation failed with error: " << WSAGetLastError());
            WSACleanup();
            return false;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port_);

        result = bind(listen_fd_, (SOCKADDR*)&server_addr, sizeof(server_addr));
        if (result == SOCKET_ERROR) {
            LOGE("bind failed with error: " << WSAGetLastError());
            closesocket(listen_fd_);
            WSACleanup();
            return false;
        }

        result = ::listen(listen_fd_, backlog);
        if (result == SOCKET_ERROR) {
            LOGE("listen failed with error: " << WSAGetLastError());
            closesocket(listen_fd_);
            WSACleanup();
            return false;
        }

        LOGD("Server is listening on port " << port_);
        return true;
    }

    SOCKET tcp_server::accept() {
        sockaddr_in client_addr;
        int client_size = sizeof(client_addr);
        SOCKET client_fd = ::accept(listen_fd_, (SOCKADDR*)&client_addr, &client_size);
        if (client_fd == INVALID_SOCKET) {
            LOGE("accept failed with error: " << WSAGetLastError());
        }
        return client_fd;
    }

    void tcp_server::close() {
        if (closesocket(listen_fd_) == SOCKET_ERROR) {
            LOGE("closesocket failed with error: " << WSAGetLastError());
        }
        WSACleanup();
        listen_fd_ = INVALID_SOCKET;
    }
}