#pragma once

#include "../libraries/Network_unit.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <Windows.h>

#include "../libraries/File_system.hpp"
#include "../libraries/Any_file.h"
#include "../libraries/Mytime.hpp"

class Handler
{
public:

	Handler(Network_unit* unit, bool state = false);

	bool do_ransaction();

private:

	bool wait_user_command();

	bool input_handler();

	bool wait_request();

	bool request_handler();

	bool send_message();

	bool get_message();

	bool send_file();

	bool get_file();

	bool change_state();

	Network_unit* unit;
	char* bufer;
	int length_message;

	std::string command;
	bool state;	// 
};