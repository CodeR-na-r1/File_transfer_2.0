﻿#include "Handler.h"

Handler::Handler(Network_unit* unit, bool state)
// Если флаг поднят, то объект будет ожидать ввода от пользователя иначе от другого участника сетевого подключения
{
	this->unit = unit;
	this->bufer = this->unit->get_bufer();
	this->length_message = 0;

	this->state = state;
}

bool Handler::do_ransaction()
{
	return this->state ? this->wait_user_command() : this->wait_request();
}

bool Handler::wait_user_command()
{
Enter_command:
	std::cout << "\n-> ";
	std::getline(std::cin, this->command);

	if (std::cin.rdbuf()->in_avail())
		std::cin.ignore(999, '\n');

	if (this->command == "")
	{
		goto Enter_command;
	}

	return this->input_handler();
}

bool Handler::input_handler()
{
	if (this->command == "--s" || this->command == "share")
	{
		this->bufer[0] = 0x1; this->bufer[1] = 0x2;
		this->unit->send_to(2);
		this->length_message = this->unit->receive_from();
		return this->length_message == 1 ? this->send_file() : false;
	}
	else if (this->command == "--p" || this->command == "pass")
	{
		this->bufer[0] = 0x1; this->bufer[1] = 0x1;
		this->unit->send_to(2);
		this->length_message = this->unit->receive_from();
		return this->length_message == 1 ? this->change_state() : false;
	}
	else if (this->command == "--e" || this->command == "exit")
	{
		this->bufer[0] = 0x1; this->bufer[1] = 0x3;
		this->unit->send_to(2);
		return false;
	}
	else
	{
		return this->send_message();
	}
}

bool Handler::wait_request()
{
	std::cout << "\n>> Waiting network request...";

	this->length_message = this->unit->receive_from();

	return this->request_handler();
}

bool Handler::request_handler()
{
	if (this->length_message == 2)
	{
		if (this->bufer[0] == 0x1 && this->bufer[1] == 0x1)
		{
			this->unit->send_to(1);
			return this->change_state();
		}
		else if (this->bufer[0] == 0x1 && this->bufer[1] == 0x2)
		{
			this->unit->send_to(1);
			return this->get_file();
		}
		else if (this->bufer[0] == 0x1 && this->bufer[1] == 0x3)
		{
			return false;
		}
	}

	return this->get_message();	// Для отправки сообщения - 1 сравнение, или в худшем редком случае - 5 сравнений
}

bool Handler::send_message()
{
	bool over_bufer = false;
	int i(0);

	do
	{
		int remains = this->command.length() - i;
		remains > this->unit->get_size_bufer() ? over_bufer = true : over_bufer = false;
		int need_send = over_bufer ? this->unit->get_size_bufer() - 1 : remains;
		if (over_bufer) this->bufer[0] = 0x3;

		for (int it = over_bufer ? 1 : 0; it < need_send; it++, i++)
		{
			this->bufer[it] = this->command[i];
		}

		this->unit->send_to(over_bufer ? this->unit->get_size_bufer() : remains);

		this->length_message = this->unit->receive_from();
		if (this->bufer[0] != 0x0) return false;

	} while (over_bufer);

	std::cout << ">> (success)";

	return true;
}

bool Handler::get_message()
{
	bool is_end = false;
	std::cout << "\n>> ";

	do
	{
		this->bufer[0] == 0x3 ? is_end = false : is_end = true;

		for (int i = is_end ? 0 : 1; i < this->length_message; i++)
		{
			std::cout << this->bufer[i];
		}

		this->bufer[0] = 0x0;
		this->unit->send_to(1);

		if (!is_end)
		{
			this->length_message = this->unit->receive_from();
			if (this->length_message <= 0) { return false; }
		}

	} while (!is_end);

	return true;
}

bool Handler::send_file()
{
	std::cout << ">> Enter name file or directory:\n-> ";
	std::getline(std::cin, this->command);

	if (std::cin.rdbuf()->in_avail())
		std::cin.ignore(999, '\n');

	if (!File_system::exists(this->command))
	{
		std::cout << "\n!> File/directory not exists (find)";
		this->unit->send_to(1);
		return false;
	}

	this->unit->send_to(2);

	std::cout << "\n~> Data processing...";

	std::vector<std::string> files, dirs;
	std::string dir_to_obj = File_system::get_dir_without_filename(this->command);
	std::string main_dir_or_file = File_system::get_filename(this->command);

	if (!File_system::is_directory(this->command))
	{
		files.push_back(main_dir_or_file);
	}
	else
	{
		dirs.push_back(main_dir_or_file);
		File_system::files_in_dir(dir_to_obj, files, dirs, main_dir_or_file + File_system::get_separator());
	}

	std::cout << "\n~> Waiting response from recipient...";

	this->length_message = this->unit->receive_from();
	if (this->length_message != 2) { std::cout << "\n!> The recipient canceled"; return false; }

	std::cout << "\n~> Sending meta information...";

	for (std::string dir : dirs)
	{
		for (int i = 0; i < dir.length(); i++)
		{
			this->bufer[i] = dir[i];
		}

		this->unit->send_to(dir.length());

		this->length_message = this->unit->receive_from();
		if (this->length_message != 1) { return false; }
	}

	this->bufer[0] = 0x1;
	this->unit->send_to(1);
	this->length_message = this->unit->receive_from();
	if (this->length_message != 2) { return false; }

	*((int*)this->bufer) = files.size();
	this->unit->send_to(sizeof(int));
	this->length_message = this->unit->receive_from();
	if (this->length_message != 3) { return false; }

	std::cout << "\n~> Send files:\n";

	std::atomic<int> sent_files(0);
	int total_files(files.size());
	Mytime time;
	std::atomic<unsigned long long> recieved(0);
	unsigned long long total_files_size = File_system::size_dir(File_system::unite_paths(dir_to_obj, main_dir_or_file));
	std::atomic<bool> is_end(false);

	std::thread t1([&is_end, &sent_files, &total_files, &recieved, &total_files_size, &time]()
		{
			Console_manipulation cm1, cm2;
			Console_show_loading loading(0, total_files_size, 20);
			cm1.init();
			while (true)
			{
				cm1.load_saved_cursor_pos();
				cm1.clear_row();
				std::cout << "~> Transferred : " << sent_files.load(std::memory_order_relaxed) << " out of " << total_files << ". Avg speed: " << recieved.load(std::memory_order_relaxed) / (double)1024 / 1024 / time.get_time() << " mb / s;";
				cm1.save_now_cursor_pos();

				std::cout << '\n';

				cm2.load_saved_cursor_pos();
				cm2.clear_row();
				loading.show_loading(recieved.load(std::memory_order_relaxed));
				cm2.save_now_cursor_pos();

				if (is_end) { break; }

				Sleep(800);
			}
			
			std::cout << '\n';
		}
	);

	for (std::string file : files)
	{
		for (int i = 0; i < file.length(); i++)
		{
			this->bufer[i] = file[i];
		}

		Any_file any_file((File_system::unite_paths(dir_to_obj, file)).c_str(), Type::input);
		if (!any_file.is_open())
		{
			std::cout << "\n!> Failed to open file!";
			this->bufer[0] = 0x2;
			this->unit->send_to(1);
			return false;
		}

		this->unit->send_to(file.length());
		this->length_message = this->unit->receive_from();
		if (this->length_message != 1) { return false; }

		*((unsigned long long*)this->bufer) = any_file.get_size();
		this->unit->send_to(sizeof(unsigned long long));
		this->length_message = this->unit->receive_from();
		if (this->length_message != 1) { return false; }

		while ((this->length_message = any_file.get_data(this->bufer, this->unit->get_size_bufer())) > 0)
		{
			this->unit->send_to(this->length_message);
			recieved.fetch_add(this->length_message, std::memory_order_relaxed);

			this->length_message = this->unit->receive_from();
			if (this->length_message != 1) { return false; }
		}
		
		sent_files.fetch_add(1, std::memory_order_relaxed);
	}

	is_end.store(true, std::memory_order_relaxed);
	t1.join();

	Console_manipulation::set_text_color(Text_color::Green);
	std::cout << "\n^> Files transferred successfully! Elapsed: " << time.get_time() << " s";
	Console_manipulation::reset_all();

	return true;
}

bool Handler::get_file()
{
	this->length_message = this->unit->receive_from();
	if (this->length_message != 2) { return false; }

	std::cout << "\n>> Accept a request for a file or directory? (y/n)\n-> ";
	std::cin >> this->command;

	if (std::cin.rdbuf()->in_avail())
		std::cin.ignore(999, '\n');
	
	if (this->command == "n" || this->command == "no") { this->unit->send_to(1); return false; }

	std::cout << "\n>> Enter the location of the new files (for dir '" << File_system::get_work_dir() << "' click enter)\n-> ";
	
	if (std::cin.get() != '\n')
	{
		std::cin.unget();
		std::getline(std::cin, this->command);

		if (std::cin.rdbuf()->in_avail())
			std::cin.ignore(999, '\n');
	}
	else
	{
		this->command = "";
	}

	if (this->command != "" && !File_system::exists(this->command)) { this->unit->send_to(1); return false; }

	std::string dir_for_files = this->command;

	this->unit->send_to(2);

	// ------ Приём мета информации ------

	std::cout << "\n~> Received meta information";

	while (true)
	{
		this->length_message = this->unit->receive_from();

		if (this->bufer[0] == 0x1) break;

		File_system::create_dir(dir_for_files, std::string(this->bufer, this->length_message));

		this->unit->send_to(1);
	}

	this->unit->send_to(2);

	// ------ Приём информации о размере файла ------

	this->length_message = this->unit->receive_from();
	if (this->length_message != sizeof(int)) { return false; }
	int total_files = *((int*)this->bufer);
	this->unit->send_to(3);

	std::cout << "\n~> Will be received " << total_files << " files\n";

	Console_manipulation cm1, cm2;
	cm1.init();

	std::string namefile;
	int get_files(0);
	unsigned long long file_size(0);
	Mytime time, timer_for_output;
	unsigned long long recieved(0);

	while (total_files != get_files)
	{
		this->length_message = this->unit->receive_from();

		if (this->bufer[0] == 0x2) break;

		namefile = std::string(this->bufer, this->length_message);

		if (true)//timer_for_output.get_time() > 0.8)
		{
			cm1.load_saved_cursor_pos();
			cm1.clear_row();
			std::cout << "~> Transferred: " << get_files << " out of " << total_files << ". Avg speed: " << recieved / (double)1024 / 1024 / time.get_time() << " mb/s;";
			cm1.save_now_cursor_pos();

			std::cout << '\n';

			cm2.load_saved_cursor_pos();
			cm2.clear_row();
			std::cout << "Now sent: " << namefile;
			cm2.save_now_cursor_pos();

			timer_for_output.retime();
		}

		Any_file any_file(File_system::unite_paths(dir_for_files, namefile).c_str(), Type::output);

		if (!any_file.is_open())
		{
			std::cout << "\n!> Failed to create file!";
			this->unit->send_to(2);
			return false;
		}

		this->unit->send_to(1);

		this->length_message = this->unit->receive_from();
		if (this->length_message != sizeof(unsigned long long)) { this->unit->send_to(2); return false; }
		file_size = *((unsigned long long*)this->bufer);
		this->unit->send_to(1);

		while (any_file.get_size() != file_size)
		{
			this->length_message = this->unit->receive_from();
			any_file.write_data(this->bufer, this->length_message);

			this->unit->send_to(1);
		}

		recieved += any_file.get_size();
		++get_files;
	}

	Console_manipulation::set_text_color(Text_color::Green);
	std::cout << "\n^> Files transferred successfully! Elapsed: " << time.get_time() << " s";
	Console_manipulation::reset_all();

	return true;
}

bool Handler::change_state()
{
	this->state = !this->state;

	return true;
}