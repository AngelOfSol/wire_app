#include "scene.h"


scene::scene(open_gl_handler& arg0):m_gl_handler(arg0)
{
}


scene::~scene(void)
{
}

int scene::getNewID()
{
	scene::idCounter++;
	int ret = scene::idCounter;
	return ret;
}
int scene::idCounter = 0;