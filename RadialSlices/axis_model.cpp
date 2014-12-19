#include "axis_model.h"
#include "shader_data.h"
#include "uniform.h"

axis_model::axis_model()
{
}


axis_model::~axis_model()
{
}


void axis_model::draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const
{
	this->m_va.bind();

	glUseProgram(axis_model::shader().id);
	auto mvp = perspective * view * model;

	uniform(axis_model::shader(), "color", std::vector<glm::vec4>
	{
		{1.0f, 0.0f, 0.0f, 1.0f}
		, { 0.0f, 1.0f, 0.0f, 1.0f }
		, { 0.0f, 0.0f, 1.0f, 1.0f }
	});
	uniform(axis_model::shader(), "mvp", mvp);

	glDrawArrays(GL_POINTS, 0, 3);

	this->m_va.unbind();
}

void axis_model::draw() const
{
	this->m_va.bind();

	glDrawArrays(GL_POINTS, 0, 3);

	this->m_va.unbind();
}

shader_program::ptr axis_model::s_shader = nullptr;

const shader_program& axis_model::shader()
{
	if (axis_model::s_shader == nullptr)
	{
		auto vertex = make_shader(shaders::axis_model::vertex, GL_VERTEX_SHADER);
		auto geometry = make_shader(shaders::axis_model::geometry, GL_GEOMETRY_SHADER);
		auto fragment = make_shader(shaders::axis_model::fragment, GL_FRAGMENT_SHADER);

		axis_model::s_shader = make_shader_program({ vertex, geometry, fragment });
	}
	return *axis_model::s_shader;
}
