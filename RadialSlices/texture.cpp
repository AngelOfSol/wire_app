#include "texture.h"
#include <iostream>

texture::texture(unsigned int width, unsigned int height, GLuint id):width(width), height(height), id(id)
{
}


texture::~texture(void)
{
}

texture::ptr make_texture(pixel_data_t data, GLenum textureNumber)
{

	// set up the texture buffers so we can use them
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(textureNumber);
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// set up a raw data array for OpenGL to copy from
	std::vector<GLfloat> raw;
	raw.reserve(data.size() * data[0].size() * pixel_t().size());
	
	for(unsigned int y = 0; y < data[0].size(); y++)
	{
		for(unsigned int x = 0; x < data.size(); x++)
		{
			auto& cell = data[x][y];
			raw.insert(raw.end(), cell.begin(), cell.end());
		}
	}
	

	// give open gl our data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.size(), data[0].size(), 0, GL_RGBA, GL_FLOAT, raw.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	// set up our texture parameters so opengl knows how to render them
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	auto ret = std::make_unique<texture>(data.size(), data[0].size(), id);
	return ret;
}