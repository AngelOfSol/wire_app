#include "regex_ext.h"

bool matches(std::string target, std::string regex)
{
	boost::regex reg(regex);
	return boost::regex_search(target, reg);
}