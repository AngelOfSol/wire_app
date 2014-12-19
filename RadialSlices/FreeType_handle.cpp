#include "FreeType_handle.h"

namespace jck
{
FreeType_handle::FreeType_handle(void)
{
	auto error = FT_Init_FreeType(&this->library_);

	if(error)
		throw FreeType_failure();

}


FreeType_handle::~FreeType_handle(void)
{
	FT_Done_FreeType(this->library_);
}

FreeType_handle::operator const FT_Library&() const
{
	return this->library_;
}
}