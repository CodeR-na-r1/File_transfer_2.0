#pragma once

#include <iostream>
#include <iomanip>

#include <string>

#include <Windows.h>

enum class Text_state
{
	Bold = 1,
	Faded = 2,
	Italics = 3,
	Underlined = 4,
	Flashing = 5,
	Crossed_out = 9,
};

enum class Text_color
{
	Black = 30,
	Red = 31,
	Green = 32,
	Yellow = 33,
	Navy = 34,
	Purple = 35,
	Turquoise = 36,
	White = 37,
};

enum class Background_color
{
	Black = 40,
	Red = 41,
	Green = 42,
	Yellow = 43,
	Navy = 44,
	Purple = 45,
	Turquoise = 46,
	White = 47,
};

class Console_manipulation
{
public:

	Console_manipulation();

	void set_size_console(const int x, const int y, const short width, const short height);

	bool init();

	void hide_cursor();

	void show_cursor();

	static void reset_all();

	void clear_row();

	static void set_text_state(const Text_state t_state);

	static void set_text_color(const Text_color t_color);

	static void set_background_color(const Background_color bg_color);

	static void set_params(const Text_state t_state, const Text_color t_color);

	static void set_params(const Text_color t_color, const Background_color bg_color);

	static void set_params(const Text_state t_state, const Text_color t_color, const Background_color bg_color);

	void set_cursor_pos(const int x, const int y);

	void set_cursor_pos_on_prev_str();

	void shift_coordinates(const int x, const int y);	// Сдвигает координаты на заданные x, y

	COORD get_now_cursor_pos();

	void save_now_cursor_pos();

	void load_saved_cursor_pos();

private:

	HANDLE hWnd;
	COORD bufferConsole;
	CONSOLE_CURSOR_INFO curs;
	CONSOLE_SCREEN_BUFFER_INFO bufer_info;
	DWORD dwMode;

	COORD saved_bufferConsole;
	bool is_saved_coord;
};

class Console_show_loading
{
public:

	Console_show_loading(unsigned long long start_value, unsigned long long finish_value);
	Console_show_loading(unsigned long long start_value, unsigned long long finish_value, unsigned long long max_quantity_divisions);
	Console_show_loading(unsigned long long start_value, unsigned long long finish_value, char filler, unsigned long long max_quantity_divisions);
	Console_show_loading(unsigned long long start_value, unsigned long long finish_value, char filler, unsigned long long max_quantity_divisions, Text_color color);

	void show_loading(const unsigned long long now_value) const;

private:

	unsigned long long start_value, finish_value;
	unsigned long long max_quantity_divisions;
	Text_color color;
	char filler;
};