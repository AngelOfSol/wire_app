#pragma once
#include "gl.h"

template <GLenum First, GLenum... Rest>
struct gl_state_on
	: public gl_state_on<First>, public gl_state_on<Rest...>
{
};

template <GLenum Flag>
struct gl_state_on<Flag>
{
public:
	gl_state_on(void)
	{
		glGetBooleanv(Flag, &this->wasSet_);
		if(this->wasSet_ == GL_FALSE)
			glEnable(Flag);
	};
	~gl_state_on(void)
	{
		if(this->wasSet_ == GL_FALSE)
			glDisable(Flag);
	};
private:
	GLboolean wasSet_;
};

template <GLenum First, GLenum... Rest>
struct gl_state_off
	: public gl_state_off<First>, public gl_state_off<Rest...>
{
};

template <GLenum Flag>
struct gl_state_off<Flag>
{
public:
	gl_state_off(void)
	{
		glGetBooleanv(Flag, &this->wasSet_);
		if(this->wasSet_ == GL_TRUE)
			glDisable(Flag);
	};
	~gl_state_off(void)
	{
		if(this->wasSet_ == GL_TRUE)
			glEnable(Flag);
	};
private:
	GLboolean wasSet_;
};