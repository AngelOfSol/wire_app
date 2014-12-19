#pragma once
#include "FreeType_handle.h"
#include <exception>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
namespace jck
{

class FreeTypeFontFailure : public std::exception
{
	virtual const char* what() const throw() override
	{
		return "FreeType font initialization failed.";
	}
};
/*
Provides an RAII based construct to handle FreeType fonts.
*/
class FreeType_font_handle
{
public:
	// Throws an exception when the font is not found or some other error happens.
	FreeType_font_handle(const FreeType_handle&, std::string fontName, int fontIndex) throw(FreeTypeFontFailure);
	~FreeType_font_handle(void);
	operator const FT_Face&() const;
	// accesses properties of FT_Face
	FT_Face operator ->();
private:
	FreeType_font_handle(const FreeType_font_handle&);
	FT_Face face_;
};

}