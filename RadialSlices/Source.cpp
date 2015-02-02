#include "gl.h"
#include "open_gl_handler.h"
#include "scene.h"
#include "command_line_arguments.h"
#include "config.h"
#include "render_scene.h"
#include "slice_model.h"
#include <iostream>
#include "windows_file_dialog.h"
#include "model_data.h"
#include "pipe.h"

open_gl_handler glHandler;

void update(void)
{
	glHandler.update();
}

void draw(void)
{
	glHandler.draw();
}
void reshape(int w, int h)
{
	glHandler.reshape(w, h);
}

void mouseDown(int x, int y)
{
	glHandler.mouseDown(x, y);
}


void mouseUp(int x, int y)
{
	glHandler.mouseUp(x, y);
}

void mouseChange(int button, int state, int x, int y)
{
	glHandler.mouseChange(button, state, x, y);
}

void asciiKeyDown(unsigned char key, int x, int y)
{
	glHandler.asciiKeyDown(key, x, y);
}
void asciiKeyUp(unsigned char key, int x, int y)
{
	glHandler.asciiKeyUp(key, x, y);
}
void keyDown(int key, int x, int y)
{
	glHandler.keyDown(key, x, y);
}
void keyUp(int key, int x, int y)
{
	glHandler.keyUp(key, x, y);
}

int main(int argc, char ** argv)
{

	auto args = rmg_com::make_args(argc, argv);

	std::string absolute = "";

	pipe_t pipe(R"(C:\simulator\y.pipe)");


	// attempt to grab hold of a mutex for the application
	auto mutex = CreateMutex(nullptr, true, "Global\\rmg_wire_viewer");
	// if it doesn't return a mutex something went wrong and we need to exit
	if (mutex == nullptr)
	{
		return 0;
	}
	else
	{
		// if it already exists send information to the previous instance
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			/***
			This is the code that remains cross-platform regardless of whether or not
			the Windows API is being used.
			(i.e. if you change the code, keep this section)
			***/
			if (args.has_option("close"))
				pipe.write("close");

			return 0;
		}
		// otherwise start the program as normal
	}

	if (args.has_option("close"))
		return 0;
	/*if (!args.has_option("r"))
		return 0;*/ // uncomment for production

	config config(R"(C:\simulator\cfg.cfg)");


	bool borderless = config.as<int>("window-borderless") == 1;

	if (args.has_option("border"))
		borderless = true;
	if (args.has_option("noborder"))
		borderless = false;


	glm::vec2 window_size(config.as<int>("wire-window-width"), config.as<int>("wire-window-height"));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL | GLUT_DOUBLE | (borderless ? GLUT_BORDERLESS : 0));
	glutInitWindowSize((int)window_size.x, (int)window_size.y);
	glutInitContextVersion(config.as<int>("gl-major"), config.as<int>("gl-minor"));
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glEnable(GLUT_MULTISAMPLE);
	glutCreateWindow("Wire Model Viewer");
	auto hwndTest = GetActiveWindow();

	glHandler.hwnd = hwndTest;

	auto x = config.as<int>("wire-window-x");
	if (args.has_option("x"))
		x = std::stoi(args["x"]);

	auto y = config.as<int>("wire-window-y");
	if (args.has_option("y"))
		x = std::stoi(args["y"]);

	if (args.has_option("w"))
		window_size.x = std::stoi(args["w"]);
	if (args.has_option("h"))
		window_size.y = std::stoi(args["h"]);

	file_dialog::handle = hwndTest;

	std::cout << glGetString(GL_VENDOR) << std::endl <<
		glGetString(GL_RENDERER) << std::endl <<
		glGetString(GL_VERSION) << std::endl <<
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// required to have for some of the extensions used here work
	glewExperimental = true;
	auto glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		return 1;
	}



	glHandler.init(window_size);

	model_data input;

	input.pipe = &pipe;

	input.config.load(R"(C:\simulator\cfg.cfg)");
	if (args.arguments().size() > 0)
		input.model_file_name = args.arguments()[0];
	else
	{
		input.model_file_name = get_file_name("Select a Recipe File");//R"(G:\EP FLEX DRESS\EP Flex NiTi paddle 0,045 SC MM.htm)";
		if (input.model_file_name == "ERROR")
		{
			MessageBox(NULL, "No part file provided.", "No File", MB_ICONINFORMATION | MB_OK | MB_SETFOREGROUND);
			return 0;

		}
	}

	input.acr = args.has_option("acr");
	input.start_live = args.has_option("live");
	auto renderScene = std::make_unique<render_scene>(glHandler, input);

	try
	{
		renderScene->init();
	}
	catch (missing_data e)
	{
		std::string message = "Your recipe had an error at: " + e.location_name;
		if (e.data_name != "NONE")
			message += "\rFor the data specifed as: " + e.data_name;
		MessageBox(NULL, message.c_str(), NULL, MB_ICONERROR | MB_OK | MB_SETFOREGROUND);
		return 0;
	}
	catch (std::exception e)
	{
		MessageBox(NULL, e.what(), NULL, MB_ICONERROR | MB_OK | MB_SETFOREGROUND);
		return 0;
	}

	glHandler.add_scene(std::move(renderScene));

	glHandler.current_scene() = render_scene::id;

	glutIdleFunc(update);

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);

	glutMouseFunc(mouseChange);
	glutMotionFunc(mouseDown);
	glutPassiveMotionFunc(mouseUp);

	glutKeyboardFunc(asciiKeyDown);
	glutKeyboardUpFunc(asciiKeyUp);
	glutSpecialFunc(keyDown);
	glutSpecialUpFunc(keyUp);
	/*
	// set up keyboard callbacks

	// set up mouse call backs

	// set up drawing and updating callbacks

	//set up window resize call backs
	*/
	if (borderless)
		SetWindowPos(hwndTest, HWND_TOPMOST, x, y, window_size.x, window_size.y, SWP_NOSIZE);
	glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();
	return 0;
}