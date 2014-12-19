#pragma once
#include "gl_va.h"
#include "shader_program.h"
#include <vector>
class tool_path_model
{
public:
	tool_path_model(const std::vector<glm::vec2>& target_data);
	~tool_path_model();
	void draw(glm::mat4);
	void update();
	static const shader_program& shader();
	static shader_program::ptr s_shader;
private:
	gl_va m_va;
	const std::vector<glm::vec2>& m_data;
};

