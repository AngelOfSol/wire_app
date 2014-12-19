#pragma once
#include "gl.h"

class gl_va
{
public:
	gl_va();
	~gl_va();
	GLuint vao;
	GLuint vbo;

	void bind() const;
	void unbind() const;
};

