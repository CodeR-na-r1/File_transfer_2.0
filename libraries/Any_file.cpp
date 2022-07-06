#include "Any_file.h"

Any_file::Any_file(const char* name, Type type)
{
	if (type == Type::input)
		this->file.open(name, std::ios::binary | std::ios::in);
	else
		this->file.open(name, std::ios::binary | std::ios::out);

	this->type = type;
	
	this->iterator = 0;

	this->file.seekg(0, std::ios::end);
	this->size = this->file.tellg();
	this->file.seekg(0, std::ios::beg);

	this->_init();

	return;
}

bool Any_file::_init() const
{
	if (this->file.is_open())
		return false;

	this->type = Type::error;

	return true;
}

bool Any_file::is_open() const
{
	return (this->type == Type::error) ? false : true;
}

signed long long int Any_file::get_data(char* bufer, const signed long long int size)
{
	if (this->type != Type::input)
		return 0;

	signed long long int length = ((this->size - this->iterator) <= size) ? this->size - this->iterator : size;

	file.read(bufer, length * sizeof(char));	// Вызываем один раз, так как цена вызова системных вызовов высока

	this->iterator += length;

	return length;
}

bool Any_file::write_data(const char* bufer, const signed long long int size)
{
	if (this->type != Type::output)
		return true;

	file.write(bufer, size);

	this->size += size;

	return false;
}

bool Any_file::eof()
{
	return (this->size == this->iterator);
}

signed long long int Any_file::get_pos() const
{
	return this->iterator;
}

void Any_file::set_pos(const signed long long int position)
{
	this->iterator = position;

	return;
}

signed long long int Any_file::get_size() const
{
	return this->size;
}

Any_file::~Any_file()
{
	file.close();
}