#pragma once
#include "glm_includes.h"
#include "clipper.hpp"
#include <vector>
class slice
{
public:
	slice(glm::vec2 initial_dimensions);
	slice(glm::vec2 initial_dimensions, std::vector<glm::vec2> radii);
	slice(glm::vec2 init_dimensions, ClipperLib::Path, glm::ivec2 scalar_by = { 1, 1 }, glm::vec2 shifted_by = {});
	~slice();
	
	// shifts then scales
	ClipperLib::Path as_int_path(glm::ivec2 scalar = { 1, 1 }, glm::vec2 shift = {}) const;
	// does not have full bounds
	std::vector<glm::vec2> as_float_path() const;

	void cut(glm::ivec2 scale_this, glm::vec2 shift_this, slice clip, glm::ivec2 scale_clip, glm::vec2 shift_clip);

	slice sweep(glm::ivec2 scale, glm::vec2 start, glm::vec2 stop) const;
	slice transform(glm::ivec2 scalar = { 1, 1 }, glm::vec2 shift = {}) const;

	int points_at(float x) const;
	void slice_at(float x);
	const glm::vec2 initial_dimensions;

	static slice union_all(std::vector<slice>, glm::ivec2 scalar);

	unsigned int size() const { return this->m_radii.size(); };

	void set(std::vector<glm::vec2> v) { this->m_radii = v; };

	std::vector<slice> cut_at(float x) const;
private:
	std::vector<glm::vec2> m_radii;
};

