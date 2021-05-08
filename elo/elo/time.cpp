#include "time.hpp"

#include <iomanip>
#include <sstream>

namespace util
{
	namespace time
	{
		std::string to_string_fixed(double val, uint8_t n = 2)
		{
			std::ostringstream out;
			out << std::setprecision(n) << std::fixed << val;
			return out.str();
		}

		std::time_t now()
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch())
				.count();
		}

		void sleep(std::time_t duration)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(duration));
		}

		std::time_t from_minutes(double minutes)
		{
			return static_cast<std::time_t>(minutes * 1000000.0 * 60.0);
		}

		std::time_t from_seconds(double seconds)
		{
			return static_cast<std::time_t>(static_cast<double>(seconds) * 1000000.0);
		}

		std::time_t from_milliseconds(double milliseconds)
		{
			return static_cast<std::time_t>(milliseconds * 1000.0);
		}

		double to_minutes(std::time_t t)
		{
			return static_cast<double>(t) / (1000000.0 * 60.0);
		}

		double to_seconds(std::time_t t)
		{
			return static_cast<double>(t) / 1000000.0;
		}

		double to_milliseconds(std::time_t t)
		{
			return static_cast<double>(t) / 1000.0;
		}

		std::string format(std::time_t microseconds)
		{
			auto milliseconds = static_cast<double>(microseconds) / 1000.0;
			if (milliseconds < 1)
			{
				return to_string_fixed(static_cast<double>(microseconds)) + "us";
			}
			auto seconds = static_cast<double>(milliseconds) / 1000.0;
			if (seconds < 1)
			{
				return to_string_fixed(milliseconds) + "ms";
			}
			auto minutes = static_cast<double>(seconds) / 60.0;
			if (minutes < 1)
			{
				return to_string_fixed(seconds) + "s";
			}
			auto hours = static_cast<double>(minutes) / 60.0;
			if (hours < 1) 
			{
				return to_string_fixed(minutes) + "m";
			}
			return to_string_fixed(hours) + "h";
		}
	}
}