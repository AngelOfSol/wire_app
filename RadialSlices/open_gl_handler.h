#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "gl.h"
#include "shader.h"
#include "shader_program.h"
#include "texture.h"
#include "font.h"
#include <glm\glm.hpp>
#include "scene.h"
#include "acr_connection.h"
#include "uniform.h"

class open_gl_handler
{
public:
	struct font_data
	{
		texture::ptr texture;
		int width;
		int height;
	};
	struct mouse_t
		: public glm::vec2
	{
		bool left;
		bool middle;
		bool right;
		mouse_t() : left(false), right(false), middle(false)
		{
		};
	};
	struct keys_t
	{
		enum state
		{
			OFF = 0x0
			, ON = 0x1
			, JUST_PRESSED = 0x3
		};

		std::vector<state> letterKeys;
		std::vector<state> otherKeys;

		state operator [](char index) const
		{
			return this->letterKeys.at(index - 'a');
		}
		state space;
		state enter;
		state backspace;
		state shift;
		state control;
		state alt;

		state up;
		state down;
		state left;
		state right;

		keys_t() :letterKeys(26), space(OFF), enter(OFF), shift(OFF), control(OFF), alt(OFF), backspace(OFF), otherKeys(256)
		{
		}
	};
	open_gl_handler(void); // done
	~open_gl_handler(void); // done

	void init(glm::vec2 winSize);

	void use_program(std::string programName); // done
	template <typename Arg>
	void set(const std::string& name, Arg argument) // done
	{
		if(this->m_current_program != "")
		{
			const auto& prog = *this->m_shaders[this->m_current_program];
			glUseProgram(prog.id);
			uniform(prog, name, argument);
		}
	}

	bool load_font(std::string filename, std::string fontName, int size); // done
	void use_font(std::string fontName, float size = 1.0f); // done
	const font_data& get_font(std::string name) const; // done
	void write(std::string data, glm::vec2 pos, bool rightOrient = true); // done
	
	glm::vec2 window_size() const; // done
	mouse_t mouse() const; // done
	const keys_t& keys() const; // done
	scene_id& current_scene();
	const scene_id& current_scene() const;
	std::string current_program() const; // done
	void add_scene(scene::ptr); // done


	void update(void); // done
	void draw(void); // done
	
	void asciiKeyUp(unsigned char, int, int); // done
	void asciiKeyDown(unsigned char, int, int); // done
	void keyDown(int, int, int); // done
	void keyUp(int, int, int); // done

	void reshape(int, int); // done

	void mouseDown(int, int); // done
	void mouseUp(int, int); // done
	void mouseChange(int, int, int, int); // done

	HWND hwnd;

	void close(void);
	
	int shaderVersion;
private:
	std::map<std::string, shader_program::ptr> m_shaders;
	std::vector<scene::ptr> m_scenes;

	acr_connection::ptr m_acr;

	std::map<std::string, font_data> m_font_list;
	std::map<std::string, texture::ptr> m_textures;
	std::string m_current_program;
	std::string m_current_font;
	scene_id m_current_scene;
	glm::vec2 m_window_size;
	keys_t m_keys;
	mouse_t m_mouse;
	int m_prev;
};

