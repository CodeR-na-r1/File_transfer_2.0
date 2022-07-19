#pragma once

#include <iostream>
#include <fstream>

enum class Type
{
	input,
	output,
	error
};

class Any_file
{
public:
	Any_file(const char* name, Type type);

	bool is_open() const;

	unsigned long long int get_data(char* bufer, const signed long long int size);

	bool write_data(const char* bufer, const signed long long int size);

	bool eof();

	unsigned long long int get_pos() const;

	void set_pos(const unsigned long long int position);

	unsigned long long int get_size() const;

	~Any_file();

private:
	Any_file() = delete;

	bool _init() const;

	std::fstream file;
	mutable Type type;
	unsigned long long int size;
	unsigned long long int iterator;
};