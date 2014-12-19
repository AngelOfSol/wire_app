#include "open_gl_handler.h"
#include <fstream>
#include <glm\matrix.hpp>
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\transform.hpp>
#include "scene.h"
#include "state_gl.h"
#include "shader_data.h"

using okey = open_gl_handler::keys_t::state;
static void update(open_gl_handler::keys_t::state& s)
{
	s = okey(s & okey::ON);
}



open_gl_handler::open_gl_handler()
{
}

void open_gl_handler::init(glm::vec2 winSize)
{
	this->m_window_size = winSize;

	this->m_prev = 0;

	auto text_frag = make_shader(shaders::text::fragment, GL_FRAGMENT_SHADER);
	auto text_geo = make_shader(shaders::text::geometry, GL_GEOMETRY_SHADER);
	auto text_vert = make_shader(shaders::text::vertex, GL_VERTEX_SHADER);

	auto text_prog = make_shader_program({ text_vert, text_geo, text_frag });

	this->m_shaders["text"] = std::move(text_prog);
}

open_gl_handler::~open_gl_handler(void)
{
}

void open_gl_handler::use_program(std::string programName)
{
	glUseProgram(this->m_shaders[programName]->id);
	this->m_current_program = programName;
}

bool open_gl_handler::load_font(std::string fontPath, std::string fontName, int size)
{
	auto newFont = jck::make_font(fontPath, size);
	if (newFont)
	{
		auto& fontData = newFont->data();

		pixel_data_t raw;
		raw.resize(fontData.size());
		for (auto& row : raw)
		{
			row.resize(fontData[0].size());
		}
		pixel_t temp;
		for (unsigned int x = 0; x < fontData.size(); x++)
		{
			auto rowSize = fontData[0].size();
			for (unsigned int y = 0; y < rowSize; y++)
			{
				for (auto& val : temp)
				{
					val = fontData[x][y];
				}
				raw[x][y] = temp;
			}
		}
		auto fontlayout = make_texture(raw, GL_TEXTURE0);
		if (fontlayout)
		{
			this->m_font_list[fontName].texture = std::move(fontlayout);
			this->m_font_list[fontName].width = newFont->width;
			this->m_font_list[fontName].height = newFont->height;
			return true;
		}
	}
	return false;

}
void open_gl_handler::use_font(std::string fontName, float size)
{
	this->m_current_font = fontName;

	font_data& data = this->m_font_list[fontName];

	gl_state_on<GL_TEXTURE_2D> textures;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.texture->id);
	glm::mat4 transform = glm::ortho(0.0f, this->window_size().x, 0.0f, this->window_size().y);

	this->use_program("text");
	this->set("transformMat", transform);
	this->set("fontBuffer", 0);
	this->set("textWidth", this->get_font(fontName).width);
	this->set("textHeight", this->get_font(fontName).height);
	this->set("textureWidth", static_cast<GLfloat>(this->get_font(fontName).width) / static_cast<GLfloat>(this->get_font(fontName).texture->width));
	this->set("textureHeight", 1.0f);



}
auto open_gl_handler::get_font(std::string name) const -> const font_data&
{
	return this->m_font_list.at(name);
}
void open_gl_handler::write(std::string data, glm::vec2 pos, bool right)
{
	gl_state_on<GL_TEXTURE_2D> on;
	gl_state_off<GL_DEPTH_TEST> off;

	GLuint vao = 0;
	GLuint vbo = 0;

	pos.x = std::floor(pos.x);
	pos.y = std::floor(pos.y);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	std::vector<GLfloat> raw;
	raw.reserve(data.size() * 5);
	int i = 0;
	int offset = (right) ? (0) : (data.size() * -this->m_font_list[this->m_current_font].width);
	for (auto c : data)
	{
		raw.push_back(pos.x + i * this->m_font_list[this->m_current_font].width + offset);

		raw.push_back(pos.y);
		raw.push_back(0.0f);
		raw.push_back((GLfloat)c / 128.0f);
		raw.push_back(0.0f);
		i++;
	}


	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * raw.size(), raw.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 3 floats long starting at 0

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	// let opengl know the second attribute is texel coordinates at 2 float long starting at the 3rd index

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// enable the attributes

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// draw
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//debug::log(vao);
	glDrawArrays(GL_POINTS, 0, data.size());
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

std::string open_gl_handler::current_program() const
{
	return this->m_current_program;
}

void open_gl_handler::update(void)
{
	int newTime = glutGet(GLUT_ELAPSED_TIME);
	auto duration = newTime - this->m_prev;
	if (this->m_prev == 0)
		duration = 0;
	this->m_prev = newTime;
	auto test1 = this->m_current_scene >= this->m_scenes.size();
	if (test1 || !this->m_scenes[this->m_current_scene])
	{
		glutExit();
	}
	else
	{
		auto newScene = this->m_scenes[this->m_current_scene]->update(duration);
		if (newScene != this->m_current_scene)
		{
			this->m_current_scene = newScene;
			auto test1 = this->m_current_scene >= this->m_scenes.size();
			auto test2 = !this->m_scenes[this->m_current_scene];
			if (test1 || test2)
			{
				this->close();
			}
			this->m_scenes[this->m_current_scene]->init();
		}
		glutPostRedisplay();
	}
	for (auto& key : this->m_keys.letterKeys)
	{
		::update(key);
	}
	for (auto& key : this->m_keys.otherKeys)
	{
		::update(key);
	}
	::update(this->m_keys.alt);
	::update(this->m_keys.shift);
	::update(this->m_keys.control);
	::update(this->m_keys.space);
	::update(this->m_keys.backspace);
	::update(this->m_keys.enter);
	::update(this->m_keys.up);
	::update(this->m_keys.down);
	::update(this->m_keys.left);
	::update(this->m_keys.right);
}
scene_id& open_gl_handler::current_scene()
{
	return this->m_current_scene;
}
const scene_id& open_gl_handler::current_scene() const
{
	return this->m_current_scene;
}
void open_gl_handler::draw(void)
{
	auto test1 = this->m_current_scene >= this->m_scenes.size();
	if (!test1)
		this->m_scenes[this->m_current_scene]->draw();
	glutSwapBuffers();
}

void open_gl_handler::reshape(int w, int h)
{
	this->m_window_size = glm::vec2(w, h);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

glm::vec2 open_gl_handler::window_size() const
{
	return this->m_window_size;
}
void open_gl_handler::mouseDown(int x, int y)
{
	this->m_mouse.x = (float)x;
	this->m_mouse.y = (float)y;
}
void open_gl_handler::mouseUp(int x, int y)
{
	this->m_mouse.x = (float)x;
	this->m_mouse.y = (float)y;
}
void open_gl_handler::mouseChange(int button, int state, int x, int y)
{
	this->m_mouse.x = (float)x;
	this->m_mouse.y = (float)y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		this->m_mouse.left = state == GLUT_DOWN;
		break;
	case GLUT_RIGHT_BUTTON:
		this->m_mouse.right = state == GLUT_DOWN;
		break;
	case GLUT_MIDDLE_BUTTON:
		this->m_mouse.middle = state == GLUT_DOWN;
		break;
	};
}
auto open_gl_handler::mouse() const -> mouse_t
{
	auto ret = this->m_mouse;
	ret.y -= this->window_size().y;
	ret.y *= -1;
	return ret;
}

void open_gl_handler::asciiKeyDown(unsigned char key, int x, int y)
{

	if (key >= 'a' && key <= 'z')
		this->m_keys.letterKeys[key - 'a'] = keys_t::JUST_PRESSED;
	else if (key >= 'A' && key <= 'Z')
		this->m_keys.letterKeys[key - 'A'] = keys_t::JUST_PRESSED;
	else if (key == (char)32)
		this->m_keys.space = keys_t::JUST_PRESSED;
	else if (key == (char)13)
		this->m_keys.enter = keys_t::JUST_PRESSED;
	else if (key == (char)8)
		this->m_keys.backspace = keys_t::JUST_PRESSED;

	this->m_keys.otherKeys[key] = keys_t::JUST_PRESSED;
}
void open_gl_handler::asciiKeyUp(unsigned char key, int x, int y)
{
	if (key >= 'a' && key <= 'z')
		this->m_keys.letterKeys[key - 'a'] = keys_t::OFF;
	else if (key >= 'A' && key <= 'Z')
		this->m_keys.letterKeys[key - 'A'] = keys_t::OFF;
	else if (key == (char)32)
		this->m_keys.space = keys_t::OFF;
	else if (key == (char)13)
		this->m_keys.enter = keys_t::OFF;
	else if (key == (char)8)
		this->m_keys.backspace = keys_t::OFF;

	this->m_keys.otherKeys[key] = keys_t::OFF;

}
void open_gl_handler::keyDown(int key, int x, int y)
{

	if (key == GLUT_KEY_UP)
	{
		this->m_keys.up = keys_t::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		this->m_keys.down = keys_t::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		this->m_keys.left = keys_t::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		this->m_keys.right = keys_t::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		this->m_keys.shift = keys_t::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_ALT_L || key == GLUT_KEY_ALT_R)
	{
		this->m_keys.alt = keys_t::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_CTRL_L || key == GLUT_KEY_CTRL_R)
	{
		this->m_keys.control = keys_t::JUST_PRESSED;
	}
}

void open_gl_handler::keyUp(int key, int x, int y)
{

	if (key == GLUT_KEY_UP)
	{
		this->m_keys.up = keys_t::OFF;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		this->m_keys.down = keys_t::OFF;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		this->m_keys.left = keys_t::OFF;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		this->m_keys.right = keys_t::OFF;
	}
	else if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		this->m_keys.shift = keys_t::OFF;
	}
	else if (key == GLUT_KEY_ALT_L || key == GLUT_KEY_ALT_R)
	{
		this->m_keys.alt = keys_t::OFF;
	}
	else if (key == GLUT_KEY_CTRL_L || key == GLUT_KEY_CTRL_R)
	{
		this->m_keys.control = keys_t::OFF;
	}
}

void open_gl_handler::add_scene(scene::ptr scene)
{
	auto id = scene->getID();
	if (this->m_scenes.size() <= id)
	{
		this->m_scenes.resize(id + 1);
	}
	this->m_scenes[id] = std::move(scene);
}

auto open_gl_handler::keys() const -> const keys_t&
{
	return this->m_keys;
}
void open_gl_handler::close(void)
{
	glutLeaveMainLoop();
	glutExit();
	std::exit(0);
}