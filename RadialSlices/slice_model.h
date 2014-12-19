#pragma once
#include "slice.h"
#include "gl_va.h"
#include "shader_program.h"
class slice_model
{
public:
	using uptr = std::unique_ptr<slice_model>;
	slice_model(slice left, slice right, glm::vec3 x_vector, glm::vec3 left_y_vector, glm::vec3 right_y_vector);
	~slice_model();

	static const shader_program& shader();

	void draw(glm::mat4 perspective, glm::mat4 view, glm::mat4 model) const;
	void draw() const;


private:

	static shader_program::ptr s_shader;

	std::vector<glm::vec3> m_vertices;
	gl_va m_va;
};


slice_model::uptr make_slice_model(slice left, slice right, glm::vec3 x_vector, glm::vec3 left_y_vector, glm::vec3 right_y_vector);