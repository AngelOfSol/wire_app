#pragma once
#include <map>
#include <vector>
#include <string>
namespace rmg_com
{

class command_line_arguments
{
public:
	~command_line_arguments(void);
	
	friend command_line_arguments make_args(int argumentCount, char** argumentValues, bool skipFirst = true);
	//friend command_line_arguments make_args(const std::string& line);

	const std::map<std::string, std::string> options() const { return this->m_options; };
	bool has_option(std::string key) const { return this->m_options.count(key) > 0; };

	const std::string& operator [](std::string t) const { return this->m_options.at(t); };
	const std::string& operator [](const char* t) const { return this->m_options.at(t); };

	const std::vector<std::string>& arguments() const { return this->m_arguments; };
	const std::string& raw() const { return this->m_raw; };
private:
	command_line_arguments(void);
	std::map<std::string, std::string>  m_options;
	std::vector<std::string> m_arguments;
	std::string m_raw;
};

}