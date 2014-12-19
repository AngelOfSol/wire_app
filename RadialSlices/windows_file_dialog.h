#pragma once
#include <string>
#include <Windows.h>

std::string get_file_name(const std::string & prompt);

struct file_dialog
{
	static HWND handle;
};