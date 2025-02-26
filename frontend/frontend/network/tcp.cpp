#include <iostream>
#include "network/tcp.h"
#include "log.h"

namespace network {
	tcp_client::tcp_client(const char* ip, unsigned short port)
	{
		ip_ = ip;
		port_ = port;
		sock_fd_ = -1;
	}

	tcp_client::~tcp_client()
	{
		if (sock_fd_)
			disconnect();
	}

	bool tcp_client::connect()
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != NO_ERROR) {
			LOGE("WSAStartup function failed with error: " << result);
			return false;
		}

		sock_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock_fd_ == INVALID_SOCKET) {
			LOGE("socket function failed with error: " << WSAGetLastError());
			WSACleanup();
			return false;
		}

		sockaddr_in client_service;
		client_service.sin_family = AF_INET;
		client_service.sin_addr.s_addr = inet_addr(ip_);
		client_service.sin_port = htons(port_);

		result = ::connect(sock_fd_, (SOCKADDR*)&client_service, sizeof(client_service));
		if (result == SOCKET_ERROR) {
			LOGE("connect function failed with error: " << WSAGetLastError());
			result = closesocket(sock_fd_);
			if (result == SOCKET_ERROR)
				LOGE("closesocket function failed with error: " << WSAGetLastError());
			sock_fd_ = 0;
			WSACleanup();
			return false;
		}

		return true;
	}

	int tcp_client::send(const char* data, int size)
	{
		if (sock_fd_ == -1) return -1;
		int was_sent = 0;

		while (was_sent < size) {
			int result = ::send(sock_fd_, data + was_sent, size - was_sent, 0);
			if (result < 1) return -1;

			was_sent += result;
		}

		return was_sent;
	}

	int tcp_client::recv(char* buffer, int size)
	{
		if (sock_fd_ == -1) return -1;

		int was_recv = 0;

		while (was_recv < size) {
			int result = ::recv(sock_fd_, buffer + was_recv, size - was_recv, 0);

			if (result < 1) return -1;
			was_recv += result;
		}

		return was_recv;
	}

	void tcp_client::disconnect()
	{
		if (closesocket(sock_fd_) == SOCKET_ERROR)
			LOGE("closesocket function failed with error: " << WSAGetLastError());

		WSACleanup();

		sock_fd_ = -1;
	}
}