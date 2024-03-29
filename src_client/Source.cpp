﻿//	Сборка с++17 и выше
// Консоль при отправке файлов лучше держать в fullscreen

#include <iostream>
#include <string>

#include "Client.h"

#include "../libraries/Handler.h"
#include "../libraries/Console_manipulation.h"

using namespace Network;

int main()
{
	Console_manipulation cm;
	cm.init();

	std::cout << "Welcome to ";
	
	cm.set_text_color(Text_color::Green);
	std::cout << "<< Client >>";
	cm.reset_all();
	std::cout << " programm";

	cm.shift_coordinates(-26, 1);
	std::cout << "by Vlados MRX (v 0.4.1)\n\n";

	std::string ip;
	int port(8458);		// Значение для порта по умолчанию
	signed long long int size_bufer(1024 * 2048);	// Значение для размера буфера по умолчанию

	std::cout << "Enter ip address of server: ";
	
	std::getline(std::cin, ip);

	if (std::cin.rdbuf()->in_avail())
		std::cin.ignore(999, '\n');
	
	if (ip == "")		// Ввод пустой строки - юзаем указанные по умолчанию значения (кроме размера буфера)
	{
		ip = "127.0.0.1";		// Значение для ip адреса по умолчанию
		goto Enter_size_bufer;				// Пропуск ввода порта
	}

	std::cout << "Enter port of server: ";
Enter_port:
	if (std::cin.get() != '\n')
	{
		std::cin.unget();
		std::cin >> port;

		if (std::cin.rdbuf()->in_avail())
			std::cin.ignore(999, '\n');
	}

Enter_size_bufer:

	std::cout << "Enter size for bufer (count of kilobytes): ";

	if (std::cin.get() != '\n')
	{
		std::cin.unget();
		std::cin >> size_bufer;
		size_bufer *= 1024;

		if (std::cin.rdbuf()->in_avail())
			std::cin.ignore(999, '\n');
	}
	
	Client* client = new Client(ip, port, size_bufer);	// Указываем ip адрес сервера и номер открытого (прослушиваемового) порта сервера

	if (client->Start())
	{
		cm.set_text_color(Text_color::Red);
		std::cerr << "!> Error with start client!";
		cm.reset_all();
		system("pause");
		return -1;
	}

	Handler h(client, true);
	
	try
	{
		while (h.do_ransaction()) {}
	}
	catch (const std::exception& e)
	{
		cm.set_text_color(Text_color::Red);
		std::cout << "!> Error! Description: " << e.what();
		cm.reset_all();
	}
	catch (...)
	{
		cm.set_text_color(Text_color::Red);
		std::cout << "!> Undefinded error!";
		cm.reset_all();
	}

	client->Stop();

	system("pause");
	return 0;
}

// Порт и размер буфера у сервера и клиента должны совпадать!!!

// <------ Подключение с помощью Radmin VPN ------>
// 
// 1. Компьютер-сервер создает сеть в Radmin VPN
// 
// 2. Компьютер-клиент подключаеться к ней (через Radmin VPN)
// 
// 3. Компьютер-сервер запускает данное приложение сервера, передавая в качестве параметра (объекту типа Server) ip адрес, скопированный из сети Radmin VPN
// 
// 4. Компьютер-клиент запускает данное приложение клиента, передавая в качестве параметра (объекту типа Client) ip адрес СЕРВЕРА, скопированный из сети Radmin VPN (или взятый из приложения сервера)
// 
// 5. Также еще надо выключить Брандмауэр Защитника Windows (ибо может мозги парить от слова совсем)
// 
// <-- END << Подключение с помощью Radmin VPN >> END -->



// <------ Подключение по локальной сети ------>
// 
// 1. Компьютер-сервер в консоли (cmd) смотрит свой ip адрес (команда: ipconfig) (Адаптер Ethernet Ethernet -> IPv4-адрес)
// 
// 2. Компьютер-сервер запускает данное приложение сервера, передавая в качестве параметра (объекту типа Server) ip адрес, скопированный из cmd
// 
// 3. Компьютер-клиент запускает данное приложение клиента, передавая в качестве параметра (объекту типа Client) ip адрес СЕРВЕРА, скопированный из cmd сервера (или взятый из приложения сервера)
// 
// 4. Также еще надо выключить Брандмауэр Защитника Windows (ибо может мозги парить от слова совсем)
// 
// <-- END << Подключение по локальной сети >> END -->