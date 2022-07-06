#pragma once

class Network_unit
{
public:
	Network_unit() {};

	virtual int _init() = 0;

	virtual int Start() = 0;
	virtual int Stop() = 0;

	virtual char* get_bufer() const = 0;
	virtual signed long long int get_size_bufer() const = 0;

	virtual int receive_from() = 0;

	virtual bool send_to(const int size) = 0;
};