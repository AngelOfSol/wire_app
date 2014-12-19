#pragma once
#include <string>
#include "shader_program.h"
template <class T>
void uniform(const shader_program& program, const char *  name, T data);
template <class T>
void uniform(const shader_program& program, std::string name, T data)
{
	uniform(program, name.c_str(), data);
}