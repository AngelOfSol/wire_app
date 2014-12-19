#include "FreeType_font_handle.h"
namespace jck
{

FreeType_font_handle::FreeType_font_handle(const FreeType_handle& handle, std::string fontName, int fontIndex)
{
	auto error = FT_New_Face(handle, fontName.c_str(), fontIndex, &this->face_);
	if(error)
		throw FreeTypeFontFailure();
}


FreeType_font_handle::~FreeType_font_handle(void)
{
	FT_Done_Face(this->face_);
}

FreeType_font_handle::operator const FT_Face &() const
{
	return this->face_;
}
FT_Face FreeType_font_handle::operator->()
{
	return this->face_;
}
}