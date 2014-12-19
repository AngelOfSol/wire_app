#include "windows_file_dialog.h"
std::string get_file_name(const std::string & prompt)
{
	const int BUFSIZE = 1024;
	char buffer[BUFSIZE] = { 0 };
	OPENFILENAME ofns = { 0 };
	ofns.hwndOwner = file_dialog::handle;
	ofns.lStructSize = sizeof(ofns);
	ofns.lpstrFile = buffer;
	ofns.nMaxFile = BUFSIZE;
	ofns.lpstrTitle = prompt.c_str();
	if (GetOpenFileName(&ofns))
		return buffer;
	else
		return "ERROR";
}
HWND file_dialog::handle;