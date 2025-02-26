#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

namespace network {
	class tcp_client {
	public:
		tcp_client(const char* ip, unsigned short port);
		~tcp_client();
		bool connect();

		int send(const char* data, int size);
		int recv(char* buffer, int size);

		void disconnect();

	private:
		const char* ip_;
		unsigned short port_;

		SOCKET sock_fd_;

	};
}