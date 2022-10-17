#pragma once

#include <iostream>
#include <chrono>

class Mytime
{
private:
	std::chrono::steady_clock::time_point start, end;
	std::chrono::duration<float> duration;

public:
	Mytime() {	// Инициализация счётчика времени + старт отсчёта
		start = std::chrono::high_resolution_clock::now();
	}

	void retime() { start = std::chrono::high_resolution_clock::now(); }	// Сброс счётчика времени

	void output_time()	// Выввод в консоль
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		std::cout << "Time is code = " << duration.count() << "s" << std::endl;
	}

	float get_time()	// Получение значения
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		return this->duration.count();
	}

	~Mytime() {
		//std::cout << "Destructor: ";
		//output_time();
	}
};