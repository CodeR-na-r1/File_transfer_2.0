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

	signed long long int get_data(char* bufer, const signed long long int size);

	bool write_data(const char* bufer, const signed long long int size);

	bool eof();

	signed long long int get_pos() const;

	void set_pos(const signed long long int position);

	signed long long int get_size() const;

	~Any_file();

private:
	Any_file() = delete;

	bool _init() const;

	std::fstream file;
	mutable Type type;
	signed long long int size;
	signed long long int iterator;
};