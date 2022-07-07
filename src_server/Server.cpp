#include "Server.h"

using namespace Network;

Server::Server(const std::string server_ip, const int server_port, const signed long long int _size_bufer) :
	Network_unit(),
	ip(server_ip),
	port(server_port),
	server_socket(INVALID_SOCKET),
	client_socket(INVALID_SOCKET),
	size_client_info{ sizeof(client_info) },
	length_message{ 0 }
{
	this->size_bufer = _size_bufer < (1024 * 300) ? (1024 * 300) : _size_bufer;
	this->bufer = new char[this->size_bufer];
}

int Server::_init()
{
	if (WSAStartup(0x0202, (WSADATA*)&this->bufer[0]))
	{
		std::cerr << "Error startup dll WSAStartup" << std::endl;
		system("pause");
		return -1;
	}

	if ((this->server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error create server_socket" << std::endl;
		WSACleanup();
		system("pause");
		return -2;
	}

	ZeroMemory((void*)&this->server_info, sizeof(this->server_info));
	ZeroMemory((void*)&this->client_info, sizeof(this->client_info));

	this->server_info.sin_family = AF_INET;
	this->server_info.sin_addr.S_un.S_addr = 0;		// 0 == ADDR_ANY, привязка к любому свободному
	this->server_info.sin_addr.S_un.S_addr = inet_addr(this->ip.c_str());		// мы привязываемся к заданному в конструкторе
	this->server_info.sin_port = htons(this->port);

	if (bind(this->server_socket, (SOCKADDR*)&this->server_info, sizeof(this->server_info)) == SOCKET_ERROR)
	{
		std::cerr << "Error bind server_socket" << std::endl;
		closesocket(this->server_socket);
		WSACleanup();
		system("pause");
		return -3;
	}

	return 0;
}

int Server::Start()
{
	if (_init())
	{
		return -1;
	}

	std::cout << "\n<-- Server has been successfully started -->" << std::endl;

	std::cout << "Server address: <" << inet_ntoa(this->server_info.sin_addr) << ">" << "\t(" << this->server_info.sin_addr.S_un.S_addr << ")" << std::endl;

	std::cout << "Server has port === " << this->port << std::endl;

	std::cout << "Server has size bufer === " << this->size_bufer / 1024 << " Kbytes" << std::endl << std::endl;

	std::cout << ">> Server wait connections...." << std::endl << std::endl;

	if (listen(this->server_socket, QUEUE_MESSAGE_SIZE) == SOCKET_ERROR)
	{
		std::cerr << "Error listen server_socket" << std::endl;
		closesocket(this->server_socket);
		WSACleanup();
		system("pause");
		return -2;
	}

	if ((this->client_socket = accept(this->server_socket, (SOCKADDR*)&this->client_info, &this->size_client_info)) < 0)
	{
		std::cerr << "Error accept with client_socket" << std::endl;
		closesocket(this->server_socket);
		WSACleanup();
		system("pause");
		return -3;
	}

	HOSTENT* hst;
	if ((hst = gethostbyaddr((char*)&this->client_info.sin_addr.S_un.S_addr, 4, AF_INET)) == nullptr)
	{
		std::cerr << "Error get info about client host by address" << std::endl;
		closesocket(this->server_socket);
		closesocket(this->client_socket);
		WSACleanup();
		system("pause");
		return -4;
	}

	std::cout << "New client connected: " << hst->h_name << " [" << inet_ntoa(this->client_info.sin_addr) << "]" << std::endl;

	return 0;
}

int Server::Stop()
{
	shutdown(this->client_socket, CF_BOTH);

	closesocket(this->server_socket);
	closesocket(this->client_socket);

	WSACleanup();

	std::cout << "\n<-- Server has been successfully stopped -->" << std::endl << std::endl;

	return 0;
}

char* Server::get_bufer() const
{
	return (char*)this->bufer;
}

signed long long int Server::get_size_bufer() const
{
	return this->size_bufer;
}

int Server::receive_from()
{
	this->length_message = recv(this->client_socket, this->bufer, this->size_bufer, 0);

	return this->length_message;
}

bool Server::send_to(const int size)
{
	if ((this->length_message = send(this->client_socket, this->bufer, size, 0)) > 0)
	{
		return true;
	}

	return false;
}

Server::~Server()
{
	this->Stop();

	delete[] this->bufer;

	return;
}