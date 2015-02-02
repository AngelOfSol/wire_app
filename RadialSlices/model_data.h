#pragma once
#include <string>
#include "config.h"
#include "glm_includes.h"
#include "pipe.h"
struct model_data
{
	std::string model_file_name;
	glm::vec2 wire_size;
	double unit_to_inches;

	pipe_t* pipe;

	config config;

	bool acr;
	bool start_live;
	struct
	{
		glm::vec2 initial_size;
		// TODO add other relevant starting information from 
		// some source as defined by arnold later on
		// the named file is in Column J row 2 (9 in base-index 0) going to need new reading
		std::string filename;
	} dress_wheel;
};