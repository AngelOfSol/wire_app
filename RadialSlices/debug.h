#pragma once

#include <string>

#define DEBUG
namespace debug
{
	template <typename T>
	void log(const T& output)
	{
		log(std::to_string(output));
	}

	void log(const std::string&);
	void log(const char*);

}