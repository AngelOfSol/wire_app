#include "tip_model.h"
#include "uniform.h"
#include "shader_data.h"

tip_model::tip_model()
{
}


tip_model::~tip_model()
{
}

void tip_model::draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const
{
	this->m_buffs.bind();

	glUseProgram(tip_model::shader().id);

	auto mvp = perspective * view * model;
	auto vp = perspective * view;
	uniform(tip_model::shader(), "mvp", mvp);
	uniform(tip_model::shader(), "vp", vp);
	uniform(tip_model::shader(), "model", model);

	glDrawArrays(GL_POINTS, 0, 1);

	this->m_buffs.unbind();
}

shader_program::ptr tip_model::s_shader = nullptr;
const shader_program& tip_model::shader()
{
	if (tip_model::s_shader == nullptr)
	{
		auto vertex = make_shader(shaders::tip_model::vertex, GL_VERTEX_SHADER);
		auto geometry = make_shader(shaders::tip_model::geometry, GL_GEOMETRY_SHADER);
		auto fragment = make_shader(shaders::tip_model::fragment, GL_FRAGMENT_SHADER);

		tip_model::s_shader = make_shader_program({ vertex, geometry, fragment });

	}
	return *tip_model::s_shader;
}