#include "Console_manipulation.h"

Console_manipulation::Console_manipulation()
{
	this->hWnd = GetStdHandle(STD_OUTPUT_HANDLE);

	this->curs = { 0 };
	this->curs.dwSize = sizeof(this->curs);

	this->dwMode = 0;

	this->bufer_info = { 0 };

	this->bufferConsole = { 0 };

	this->is_saved_coord = false;
	this->saved_bufferConsole = { 0 };

	return;
}

bool Console_manipulation::init()
{
	GetConsoleMode(this->hWnd, &this->dwMode);
	this->dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;	// Для корректного отборжания виртуальных последовательностей консоли
	this->dwMode |= DISABLE_NEWLINE_AUTO_RETURN;	// Для корректной смены позиционирования каретки консоли
	return SetConsoleMode(this->hWnd, this->dwMode);	// Применяем
}

void Console_manipulation::set_size_console(const int x, const int y, const short width, const short height)
{
	this->bufferConsole = { width, height };	// Фиксированное окно консоли
	SetConsoleScreenBufferSize(hWnd, bufferConsole);

	MoveWindow(GetConsoleWindow(), x, y, width, height, TRUE);

	return;
}

void Console_manipulation::hide_cursor()
{
	this->curs.bVisible = FALSE;	// Скрыть курсор консоли
	SetConsoleCursorInfo(this->hWnd, &this->curs);	// Применяем

	return;
}

void Console_manipulation::show_cursor()
{
	this->curs.bVisible = TRUE;	// Показать курсор консоли
	SetConsoleCursorInfo(this->hWnd, &this->curs);	// Применяем

	return;
}

// Начинается управляющая последовательность с любого из этих трёх представлений : \x1b[(hex) или \u001b[(Unicode) или \033[(oct)
// Далее следуют аргументы, разделённые между собой; (можно указывать в любом порядке)
// В конце ставится буква m

void Console_manipulation::reset_all()
{
	std::cout << "\x1b[0m";
}

void Console_manipulation::set_text_state(const Text_state t_state)
{
	std::cout << "\x1b[" + std::to_string(static_cast<int>(t_state)) + "m";
}

void Console_manipulation::set_text_color(const Text_color t_color)
{
	std::cout << "\x1b[" + std::to_string(static_cast<int>(t_color)) + "m";
}

void Console_manipulation::set_background_color(const Background_color bg_color)
{
	std::cout << "\x1b[" + std::to_string(static_cast<int>(bg_color)) + "m";
}

void Console_manipulation::set_params(const Text_state t_state, const Text_color t_color)
{
	std::cout << "\x1b[" + std::to_string(static_cast<int>(t_state)) + ";" + std::to_string(static_cast<int>(t_color)) + "m";
}

void Console_manipulation::set_params(const Text_color t_color, const Background_color bg_color)
{
	std::cout << "\x1b[" + std::to_string(static_cast<int>(t_color)) + ";" + std::to_string(static_cast<int>(bg_color)) + "m";
}

void Console_manipulation::set_params(const Text_state t_state, const Text_color t_color, const Background_color bg_color)
{
	std::cout << "\x1b[" + std::to_string(static_cast<int>(t_state)) + ";" + std::to_string(static_cast<int>(t_color)) + ";" + std::to_string(static_cast<int>(bg_color)) + "m";
}

void Console_manipulation::set_cursor_pos(const int x, const int y)
{
	this->bufferConsole.X = x;
	this->bufferConsole.Y = y;

	SetConsoleCursorPosition(this->hWnd, this->bufferConsole);
}

void Console_manipulation::set_cursor_pos_on_prev_str()
{
	GetConsoleScreenBufferInfo(this->hWnd, &this->bufer_info);

	this->bufferConsole = this->bufer_info.dwCursorPosition;

	--this->bufferConsole.Y;
	this->bufferConsole.X = 0;

	SetConsoleCursorPosition(this->hWnd, this->bufferConsole);
}

void Console_manipulation::shift_coordinates(const int x, const int y)	// Сдвигает координаты на заданные x, y
{
	GetConsoleScreenBufferInfo(this->hWnd, &this->bufer_info);

	this->bufferConsole = this->bufer_info.dwCursorPosition;

	this->bufferConsole.X += x;
	this->bufferConsole.Y += y;

	SetConsoleCursorPosition(this->hWnd, this->bufferConsole);
}

COORD Console_manipulation::get_now_cursor_pos()
{
	GetConsoleScreenBufferInfo(this->hWnd, &this->bufer_info);

	return this->bufer_info.dwCursorPosition;
}

void Console_manipulation::save_now_cursor_pos()
{
	GetConsoleScreenBufferInfo(this->hWnd, &this->bufer_info);

	this->saved_bufferConsole = this->bufer_info.dwCursorPosition;
	this->is_saved_coord = true;
}

void Console_manipulation::load_saved_cursor_pos()
{
	if (this->is_saved_coord)
	{
		SetConsoleCursorPosition(this->hWnd, this->saved_bufferConsole);
	}
}