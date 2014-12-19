#include "uniform.h"
#include "gl.h"
template <>
void uniform(const shader_program& program, const char* name, glm::mat4 data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniformMatrix4fv(loc, 1, false, &data[0][0]);
}

template <>
void uniform(const shader_program& program, const char* name, int data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform1i(loc, data);
}

template <>
void uniform(const shader_program& program, const char* name, float data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform1f(loc, data);
}

template <>
void uniform(const shader_program& program, const char* name, unsigned int data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform1i(loc, data);
}

template <>
void uniform(const shader_program& program, const char* name, glm::vec3 data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform3fv(loc, 1, &data[0]);
}

template <>
void uniform(const shader_program& program, const char* name, glm::vec2 data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform2fv(loc, 1, &data[0]);
}

template <>
void uniform(const shader_program& program, const char* name, glm::ivec2 data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform2iv(loc, 1, &data[0]);
}
template <>
void uniform(const shader_program& program, const char* name, const std::vector<glm::vec3> data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform3fv(loc, data.size(), &data[0][0]);
}
template <>
void uniform(const shader_program& program, const char* name, const std::vector<glm::vec4> data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform4fv(loc, data.size(), &data[0][0]);
}

template <>
void uniform(const shader_program& program, const char* name, glm::vec4 data)
{
	auto loc = glGetUniformLocation(program.id, name);
	glUniform4fv(loc, 1, &data[0]);
}