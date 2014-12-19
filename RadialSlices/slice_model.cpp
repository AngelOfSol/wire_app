#include "slice_model.h"
#include "shader_data.h"
#include "uniform.h"
#include <algorithm>
slice_model::slice_model(slice left, slice right, glm::vec3 x_vector, glm::vec3 left_y_vector, glm::vec3 right_y_vector)
{
	auto left_path = left.as_float_path();
	auto right_path = right.as_float_path();

	if (left_path.size() != right_path.size())
		int a = 0;

	auto fix = [](decltype(left_path)& path)
	{
		auto zero_point = std::find(path.begin(), path.end(), glm::vec2{ 0, 0 });
		if (zero_point != path.end() && zero_point != path.begin())
		{
			std::rotate(path.begin(), zero_point, path.end());

		}
		if (path.back().y != 0 && path[1].y == 0)
		{
			std::rotate(path.begin(), path.begin() + 1, path.end());
			std::reverse(path.begin(), path.end());
		}
	};

	fix(left_path);
	fix(right_path);

	left.set(left_path);
	right.set(right_path);

	for (unsigned int i = 0; i < left_path.size(); i++)
	{
		int left_points = left.points_at(left_path[i].x);
		int right_points = right.points_at(left_path[i].x);
		while (left_points > right_points)
		{
			right.slice_at(left_path[i].x);
			right_points = right.points_at(left_path[i].x);
		}
	}
	for (unsigned int i = 0; i < right_path.size(); i++)
	{
		int left_points = left.points_at(right_path[i].x);
		int right_points = right.points_at(right_path[i].x);
		while (right_points > left_points)
		{
			left.slice_at(right_path[i].x);
			left_points = left.points_at(right_path[i].x);
		}
	}

	left_path = left.as_float_path();
	right_path = right.as_float_path();

	if (left_path.size() != right_path.size())
		int stop = 0;

	for (unsigned int i = 0; i < left_path.size(); i++)
	{
		auto new_left_vec = left_path[i].x * x_vector + left_path[i].y * left_y_vector;
		auto new_right_vec = right_path[i].x * x_vector + right_path[i].y * right_y_vector;

		if (left_path[i].y > 0)
		{
			this->m_vertices.push_back(new_left_vec);
			this->m_vertices.push_back(new_right_vec);
		}
		else
		{
			this->m_vertices.push_back(new_right_vec);
			this->m_vertices.push_back(new_left_vec);
		}
	}
	this->m_vertices.push_back(this->m_vertices[0]);
	this->m_vertices.push_back(this->m_vertices[1]);

	for (unsigned int i = 0; i < left_path.size(); i++)
	{
		auto new_left_vec = left_path[i].x * x_vector + left_path[i].y * left_y_vector;
		auto new_right_vec = right_path[i].x * x_vector + right_path[i].y * right_y_vector;
		if (left_path[i].y > 0)
		{
			this->m_vertices.push_back(new_right_vec);
			this->m_vertices.push_back(new_left_vec);
		}
		else
		{
			this->m_vertices.push_back(new_left_vec);
			this->m_vertices.push_back(new_right_vec);
		}
	}
	auto new_left_vec = left_path[0].x * x_vector + left_path[0].y * left_y_vector;
	auto new_right_vec = right_path[0].x * x_vector + right_path[0].y * right_y_vector;
	this->m_vertices.push_back(new_right_vec);
	this->m_vertices.push_back(new_left_vec);
	this->m_va.bind();

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * this->m_vertices.size(), &this->m_vertices.data()[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)(0));

	glEnableVertexAttribArray(0);

	this->m_va.unbind();
}


slice_model::~slice_model()
{
}


void slice_model::draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const
{
	this->m_va.bind();

	glUseProgram(slice_model::shader().id);
	auto mvp = perspective * view * model;

	uniform(slice_model::shader(), "mvp", mvp);
	uniform(slice_model::shader(), "model", model);

	uniform(slice_model::shader(), "half_way", this->m_vertices.size() / 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->m_vertices.size());

	this->m_va.unbind();
}

void slice_model::draw() const
{
	this->m_va.bind();
	
	uniform(slice_model::shader(), "half_way", this->m_vertices.size() / 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->m_vertices.size());

	this->m_va.unbind();
}

shader_program::ptr slice_model::s_shader = nullptr;

const shader_program& slice_model::shader()
{
	if (slice_model::s_shader == nullptr)
	{
		auto vertex = make_shader(shaders::slice_model::vertex, GL_VERTEX_SHADER);
		auto geometry = make_shader(shaders::slice_model::geometry, GL_GEOMETRY_SHADER);
		auto fragment = make_shader(shaders::slice_model::fragment, GL_FRAGMENT_SHADER);

		slice_model::s_shader = make_shader_program({ vertex, geometry, fragment });
	}
	return *slice_model::s_shader;
}

slice_model::uptr make_slice_model(slice left, slice right, glm::vec3 x_vector, glm::vec3 left_y_vector, glm::vec3 right_y_vector)
{
	return std::make_unique<slice_model>(left, right, x_vector, left_y_vector, right_y_vector);
}