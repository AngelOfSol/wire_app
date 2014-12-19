#pragma once
#include <string>
class pipe_t
{
public:
	pipe_t(std::string pipeLocation);
	~pipe_t();
	void write(std::string);
	std::string get();
	bool check();
private:
	std::string const m_location;
	std::string m_data;
};

