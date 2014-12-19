#include "gl_va.h"

gl_va::gl_va()
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
}
gl_va::~gl_va()
{
	glDeleteBuffers(1, &this->vbo);
	glDeleteVertexArrays(1, &this->vao);
}
void gl_va::bind() const
{
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
}
void gl_va::unbind() const
{
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}