#pragma once
#include <vector>
#include "glm_includes.h"
#include "slice.h"
#include "slice_model.h"
#include <stack>

class sliced_cylinder
{
public:
	sliced_cylinder(glm::vec2 initial_dimensions, int resolution);
	sliced_cylinder(slice starting_slice, int resolution);
	~sliced_cylinder();

	void draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const;
	void radial_cut(glm::ivec2 scale_this, glm::vec2 shift_this, slice clip_slice, glm::ivec2 scale_clip, glm::vec2 shift_clip);
	void flat_cut(glm::ivec2 scale_this, glm::vec2 shift_this, slice clip_slice, glm::ivec2 scale_clip, glm::vec2 shift_clip, double angle_offset = 0.0);

	void push();
	void pop();

	void update_models();

	const int resolution;
private:
	std::vector<slice> m_slices;
	std::stack<std::vector<slice>> m_states;
	std::vector<slice_model::uptr> m_models;
};

