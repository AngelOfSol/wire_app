#pragma once
#include "shader.h"
#include "debug.h"
#include <vector>
class shader_program
{
public:
	using ptr = std::unique_ptr<shader_program>;

	shader_program(void);
	shader_program(const shader_program&) = delete;
	shader_program& operator =(const shader_program&) = delete;
	~shader_program(void);

	const GLuint id;
};

shader_program::ptr make_shader_program(const std::vector<shader::ptr>&);
