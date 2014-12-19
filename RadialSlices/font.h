#pragma once
#include <memory>
#include <string>
#include <vector>

namespace jck
{

class font
{
public:
	using ptr = std::unique_ptr<font>;
	using data_t = std::vector<std::vector<float>>;
	const int width;
	const int height;
	font(int width, int height, int length);
	~font(void);
	auto data() -> font::data_t const { return this->m_data; };
private:
	font::data_t m_data;
	friend font::ptr make_font(std::string, int, int);
};

/*
	Attempts to construct a font from a given font path.
	Returns null if an error has occured.
	Otherwise returns a shared_ptr to a font.
*/
font::ptr make_font(std::string fontPath, int width, int height);
font::ptr make_font(std::string fontPath, int size);

}