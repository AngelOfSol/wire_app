#include "shader_program.h"

shader_program::shader_program()
	: id(glCreateProgram())
{
}


shader_program::~shader_program()
{
	glDeleteProgram(this->id);
}

shader_program::ptr make_shader_program(const std::vector<shader::ptr>& shaders)
{
	auto ret = std::make_unique<shader_program>();
	for (auto shader : shaders)
	{
		if (!shader)
		{
			debug::log("null shader passed in");
			return shader_program::ptr();
		}
		glAttachShader(ret->id, shader->id);
	}

	GLint compileStatus;

	// link the program
	glLinkProgram(ret->id);

	// check to see if it worked
	glGetProgramiv(ret->id, GL_LINK_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		char buffer[256];

		GLint logLength;
		glGetProgramInfoLog(ret->id, sizeof(buffer), &logLength, buffer);

		debug::log("in shader_program.cpp");
		debug::log(buffer);

		return shader_program::ptr();
	}
	else
	{
		return ret;
	}
}