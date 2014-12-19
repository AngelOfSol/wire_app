#pragma once
#include "gl_va.h"
#include "shader_program.h"
class axis_model
{
public:
	axis_model();
	~axis_model();

	static const shader_program& shader();

	void draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const;
	void draw() const;

private:

	static shader_program::ptr s_shader;

	gl_va m_va;
};

