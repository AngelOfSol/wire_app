#include "debug.h"

namespace debug
{

	void log(const char* output)
	{
		log(std::string(output));
	}
}


#ifdef DEBUG
#include <iostream>
namespace debug
{
	void log(const std::string& output)
	{
		std::cout << output << std::endl;
	}

}
#else
namespace debug
{
	void log(const std::string& output)
	{
	}

}
#endif