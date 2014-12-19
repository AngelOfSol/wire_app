#pragma once
#include <exception>
#include <ft2build.h>
#include FT_FREETYPE_H
namespace jck
{

class FreeType_failure : public std::exception
{
	virtual const char* what() const throw() override
	{
		return "FreeType initialization failed.";
	}
};

/*
Provides an RAII based construct to handle FreeType libraries.
*/
class FreeType_handle
{
public:
	// Throws if something went wrong during library intializiation.
	FreeType_handle(void) throw(FreeType_failure);

	~FreeType_handle(void);
	operator const FT_Library&() const;
private:
	FreeType_handle(const FreeType_handle&);
	FT_Library library_;

};

}