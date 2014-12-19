#include "sliced_cylinder.h"
#include "angle.h"
#include "uniform.h"
sliced_cylinder::sliced_cylinder(glm::vec2 initial_dimensions, int resolution)
	: resolution(resolution)
{

	for (auto i = 0; i < this->resolution; i++)
	{
		this->m_slices.emplace_back(initial_dimensions);
	}
	for (int i = 0; i < this->resolution; i++)
	{
		const auto& slice_1 = this->m_slices[i];
		const auto& slice_2 = this->m_slices[(i + 1) % this->resolution];
		auto angle_1 = static_cast<double>(i) / this->resolution * two_pi;
		auto angle_2 = static_cast<double>(i + 1) / this->resolution * two_pi;
		this->m_models.push_back(make_slice_model(slice_1, slice_2, { 1, 0, 0 }, { 0, sin(angle_1), cos(angle_1) }, { 0, sin(angle_2), cos(angle_2) }));
	}
}
sliced_cylinder::sliced_cylinder(slice starting_slice, int resolution)
	: resolution(resolution)
{
	for (auto i = 0; i < this->resolution; i++)
	{
		this->m_slices.push_back(starting_slice);
	}
	for (int i = 0; i < this->resolution; i++)
	{
		const auto& slice_1 = this->m_slices[i];
		const auto& slice_2 = this->m_slices[(i + 1) % this->resolution];
		auto angle_1 = static_cast<double>(i) / this->resolution * two_pi;
		auto angle_2 = static_cast<double>(i + 1) / this->resolution * two_pi;
		this->m_models.push_back(make_slice_model(slice_1, slice_2, { 1, 0, 0 }, { 0, sin(angle_1), cos(angle_1) }, { 0, sin(angle_2), cos(angle_2) }));
	}
}


sliced_cylinder::~sliced_cylinder()
{
}

void sliced_cylinder::draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const
{
	glUseProgram(slice_model::shader().id);
	auto mvp = perspective * view * model;

	uniform(slice_model::shader(), "mvp", perspective * view * model);
	uniform(slice_model::shader(), "model", model);

	for (const auto& models : this->m_models)
	{
		models->draw();
	}
}
void sliced_cylinder::radial_cut(glm::ivec2 scale_this, glm::vec2 shift_this, slice clip_slice, glm::ivec2 scale_clip, glm::vec2 shift_clip)
{
	for (auto& slice : this->m_slices)
	{
		slice.cut(scale_this, shift_this, clip_slice, scale_clip, shift_clip);

	}
}

void sliced_cylinder::push()
{
	this->m_states.push(this->m_slices);
}

void sliced_cylinder::pop()
{
	this->m_slices.clear();
	for (auto slice : this->m_states.top())
	{
		this->m_slices.push_back(slice);
	}
	this->m_states.pop();
}

void sliced_cylinder::update_models()
{
	this->m_models.clear();
	auto two_pi = 3.1415962535 * 2;



	for (int i = 0; i < this->resolution; i++)
	{
		const auto& slice_1 = this->m_slices[i];
		const auto& slice_2 = this->m_slices[(i + 1) % this->resolution];
		auto angle_1 = static_cast<double>(i) / this->resolution * two_pi;
		auto angle_2 = static_cast<double>(i + 1) / this->resolution * two_pi;
		this->m_models.push_back(make_slice_model(slice_1, slice_2, { 1, 0, 0 }, { 0, sin(angle_1), cos(angle_1) }, { 0, sin(angle_2), cos(angle_2) }));
	}
}
void sliced_cylinder::flat_cut(glm::ivec2 scale_this, glm::vec2 shift_this, slice clip_slice, glm::ivec2 scale_clip, glm::vec2 shift_clip, double angle_offset)
{
	for (int i = 0; i < this->m_slices.size(); i++)
	{
		auto angle = static_cast<double>(i) / this->resolution * two_pi + angle_offset;

		angle = normalize_angle(angle);

		if (angle >= two_pi / 4 && angle <= 3 * two_pi / 4)
		{
			auto& slice_ref = this->m_slices[i];
			auto projected = glm::vec2{ std::cos(angle), std::sin(angle) };
			if (projected.x != 0)
			{
				auto slope = abs(projected.y / projected.x);
				auto cos_theta = abs(atan2(projected.y, projected.x));
				
				if (slope <= 1000000000L) // max slope to allow clipper not to operator on ridiculous numbers
				{

					std::vector<glm::vec2> points;
					for (auto v : clip_slice.as_float_path())
					{
						auto tx = v.y;
						auto ny = tx * slope;
						v.y = glm::length(glm::vec2{ tx, ny });
						points.push_back(v);
					}
					slice projected_slice(clip_slice.initial_dimensions, points);
					/*
					make projected slice
					*/
					slice_ref.cut(scale_this, shift_this, projected_slice, scale_clip, shift_clip);
				}

			}
		}


	}
}