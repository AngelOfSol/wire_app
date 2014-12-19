#pragma once
#include "gl.h"
#include "debug.h"
#include <string>
#include <memory>

class shader
{
public:

	using ptr = std::shared_ptr<shader>;

	shader(GLuint shaderType);
	shader(const shader&) = delete;
	shader(shader&&) = delete;
	~shader();

	const GLuint id;
	const GLuint type;

};

shader::ptr make_shader(const std::string& data, GLuint shaderType);
