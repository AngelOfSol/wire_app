#pragma once

#include "gl_va.h"
#include "shader_program.h"
class tip_model
{
public:
	tip_model();
	~tip_model();

	glm::vec3 pos;

	void draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const;

	static const shader_program& shader();
private:
	static shader_program::ptr s_shader;
	gl_va m_buffs;
};

