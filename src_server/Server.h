#pragma once

#pragma warning(disable: 4996)

#include "../libraries/Network_unit.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "../libraries/Any_file.h"
#include "../libraries/File_system.hpp"

#define QUEUE_MESSAGE_SIZE 3

#pragma comment(lib, "Ws2_32.lib")

namespace Network
{
	class Server : public Network_unit
	{
	public:
		Server(const std::string server_ip, const int server_port, const signed long long int _size_bufer = 1024 * 2048);

		virtual int _init() override;

		virtual int Start() override;
		virtual int Stop() override;

		virtual char* get_bufer() const override;
		virtual signed long long int get_size_bufer() const override;

		virtual int receive_from() override;

		virtual bool send_to(const int size) override;

		~Server();

	private:
		Server() = delete;

		SOCKET server_socket;
		sockaddr_in server_info;

		SOCKET client_socket;
		sockaddr_in client_info;
		int size_client_info;

		std::string ip;
		int port;

		char* bufer;
		signed long long int size_bufer;
		int length_message;
	};
}