#pragma once
#include <memory>
#include <vector>
#include <glm\glm.hpp>
#include <functional>
#include <array>
#include "gl.h"

;
typedef std::array<GLfloat, 4> pixel_t;
typedef std::vector<std::vector<pixel_t>> pixel_data_t;

class texture
{
public:
	using ptr = std::unique_ptr<texture>;
	texture(unsigned int, unsigned int, GLuint);
	~texture(void);
	const unsigned int width;
	const unsigned int height;
	const GLuint id;
private:
	friend texture::ptr make_texture(pixel_data_t data, GLenum textureNumber);
};
