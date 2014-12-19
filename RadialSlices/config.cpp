#include "config.h"
#include "typedefs.h"
#include <fstream>

config::config()
{
}


config::config(std::string filename)
{
	std::ifstream data(filename);
	std::string line;
	while(std::getline(data, line))
	{
		auto keyEnd = std::find(line.begin(), line.end(), '=');
		this->data_[std::string(line.begin(), keyEnd)] = std::string(keyEnd + 1, line.end());
	}
}


config::~config(void)
{
}

template <>
std::string config::as(std::string key) const
{
	return this->data_.at(key);
}
template <>
int config::as(std::string key) const
{
	return std::stoi(this->data_.at(key));
}
template <>
double config::as(std::string key) const
{
	return std::stod(this->data_.at(key));
}

template <>
int_frac config::as(std::string key) const
{
	return stor<base_int>(this->data_.at(key));
}

template <>
void config::set(std::string key, std::string value)
{
	this->data_[key] = value;
}
template <>
void config::set(std::string key, int value)
{
	this->data_[key] = std::to_string(value);
}
template <>
void config::set(std::string key, double value)
{
	this->data_[key] = std::to_string(value);
}

bool config::save(std::string filename)
{
	std::ofstream out(filename);
	for(auto pairs : this->data_)
	{
		out << pairs.first << "=" << pairs.second << std::endl;
	}
	return true;
}
bool config::load(std::string filename, bool overwrite)
{
	if(overwrite)
		this->data_.clear();
	
	std::ifstream data(filename);
	std::string line;
	while(std::getline(data, line))
	{
		auto keyEnd = std::find(line.begin(), line.end(), '=');
		this->data_[std::string(line.begin(), keyEnd)] = std::string(keyEnd + 1, line.end());
	}
	return true;
}
//
//template <>
//int_frac config::as(std::string key) const
//{
//	return stor<base_int>(this->data_.at(key));
//}