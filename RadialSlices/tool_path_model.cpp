#include "tool_path_model.h"
#include "uniform.h"
#include "shader_data.h"

tool_path_model::tool_path_model(const std::vector<glm::vec2>& target)
	: m_data(target)
{
	this->update();
}


tool_path_model::~tool_path_model()
{
}

void tool_path_model::draw(glm::mat4 transforn)
{
	this->m_va.bind();

	glUseProgram(tool_path_model::shader().id);

	uniform(tool_path_model::shader(), "mvp", transforn);

	glDrawArrays(GL_LINE_STRIP, 0, this->m_data.size());

	this->m_va.unbind();
}
void tool_path_model::update()
{
	this->m_va.bind();

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * this->m_data.size(), &this->m_data.data()[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)(0));

	glEnableVertexAttribArray(0);

	this->m_va.unbind();
}
shader_program::ptr tool_path_model::s_shader = nullptr;

const shader_program& tool_path_model::shader()
{
	if (tool_path_model::s_shader == nullptr)
	{
		auto vertex = make_shader(shaders::tool_path::vertex, GL_VERTEX_SHADER);
		auto fragment = make_shader(shaders::tool_path::fragment, GL_FRAGMENT_SHADER);

		tool_path_model::s_shader = make_shader_program({ vertex, fragment });
	}
	return *tool_path_model::s_shader;
}