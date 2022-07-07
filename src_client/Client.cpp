#include "Client.h"

using namespace Network;

Client::Client(const std::string server_ip, const int server_port, const signed long long int _size_bufer) :
	Network_unit(),
	ip(server_ip),
	port(server_port),
	client_socket(INVALID_SOCKET),
	length_message{ 0 }
{
	this->size_bufer = _size_bufer < (1024 * 300) ? (1024 * 300) : _size_bufer;
	this->bufer = new char[this->size_bufer];
}

int Client::_init()
{
	if (WSAStartup(0x0202, (WSADATA*)&this->bufer[0]))
	{
		std::cerr << "Error startup dll WSAStartup" << std::endl;
		system("pause");
		return -1;
	}

	if ((this->client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error create client_socket" << std::endl;
		WSACleanup();
		system("pause");
		return -2;
	}

	ZeroMemory((void*)&this->server_info, sizeof(this->server_info));

	this->server_info.sin_family = AF_INET;
	this->server_info.sin_addr.S_un.S_addr = 0;
	this->server_info.sin_port = htons(this->port);

	HOSTENT* hst;

	if (inet_addr(this->ip.c_str()) != INADDR_NONE)
	{
		this->server_info.sin_addr.S_un.S_addr = inet_addr(this->ip.c_str());
	}
	else
	{
		hst = gethostbyname(this->ip.c_str());
		if (hst != nullptr)
		{
			this->server_info.sin_addr.S_un.S_addr = ((unsigned long**)hst->h_addr_list)[0][0];		// (unsigned int)
		}
		else
		{
			std::cerr << GetLastError();
			closesocket(client_socket);
			WSACleanup();
			return -3;
		}
	}

	return 0;
}

int Client::Start()
{
	if (_init())
	{
		return -1;
	}

	if (connect(this->client_socket, (SOCKADDR*)&this->server_info, sizeof(this->server_info)))
	{
		std::cerr << "Error connect with server_socket" << std::endl;
		closesocket(this->client_socket);
		WSACleanup();
		system("pause");
		return -2;
	}

	std::cout << "<-- Client has been successfully connected -->" << std::endl;

	std::cout << "<-- Client has size bufer === " << this->size_bufer / 1024 << " Kbytes" << std::endl << std::endl;

	return 0;
}

int Client::Stop()
{
	shutdown(this->client_socket, CF_BOTH);

	closesocket(this->client_socket);

	WSACleanup();

	std::cout << "\n<-- Client has been successfully disconnected -->" << std::endl << std::endl;

	return 0;
}

char* Client::get_bufer() const
{
	return (char*)this->bufer;
}

signed long long int Client::get_size_bufer() const
{
	return this->size_bufer;
}

bool Client::send_to(const int size)
{
	if ((this->length_message = send(this->client_socket, this->bufer, size, 0)) > 0)
	{
		return true;
	}

	return false;
}

int Client::receive_from()
{
	this->length_message = recv(this->client_socket, this->bufer, this->size_bufer, 0);

	return this->length_message;
}

Client::~Client()
{
	this->Stop();

	delete[] this->bufer;

	return;
}