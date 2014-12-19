#pragma once
#include <string>
#include <map>
class config
{
public:
	config(void);
	config(std::string filename);
	~config(void);
	template <typename T>
	T as(std::string key) const;
	template <typename T>
	void set(std::string key, T value);
	bool save(std::string);
	bool load(std::string, bool overwrite = true);
private:
	std::map<std::string, std::string> data_;
};

