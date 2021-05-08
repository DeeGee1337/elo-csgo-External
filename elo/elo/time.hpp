#pragma once

#include <chrono>
#include <ctime>
#include <string>
#include <thread>

namespace util 
{
	namespace time
	{
		std::time_t now();

		
		void sleep(std::time_t time);

		std::time_t from_minutes(double minutes);
		std::time_t from_seconds(double seconds);
		std::time_t from_milliseconds(double milliseconds);

		double to_minutes(std::time_t time);
		double to_seconds(std::time_t time);
		double to_milliseconds(std::time_t time);

		std::string format(std::time_t time);
	}
}