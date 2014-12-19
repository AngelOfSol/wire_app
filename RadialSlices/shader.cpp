#include "shader.h"
shader::shader(GLuint shaderType) 
	: type(shaderType)
	, id(glCreateShader(shaderType))
{

}

shader::~shader()
{
	debug::log("shader destructed");
	glDeleteShader(this->id);
	
}



shader::ptr make_shader(const std::string& fileData, GLuint shaderType)
{
	shader::ptr ret;

	if (fileData != "")
	{
		ret = std::make_shared<shader>(shaderType);
		auto data = fileData.data();
		glShaderSource(ret->id, 1, &data, nullptr);
		glCompileShader(ret->id);

		// grab the compile log if it fails
		GLint rivResults;

		glGetShaderiv(ret->id, GL_COMPILE_STATUS, &rivResults);

		if (rivResults == GL_FALSE)
		{
			char log[256];
			GLint logLength;

			glGetShaderInfoLog(ret->id, sizeof(log), &logLength, log);

			debug::log("in shader.cpp");
			debug::log(fileData);
			debug::log("");
			debug::log(log);

			return shader::ptr();
		}
		return ret;
	}
	else
	{
		return shader::ptr();
	}

}