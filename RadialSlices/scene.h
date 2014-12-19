#pragma once
#include <string>
#include <memory>

typedef unsigned int scene_id;

class open_gl_handler;
class scene
{
public:
	using ptr = std::unique_ptr<scene>;
	scene(open_gl_handler&);
	~scene(void);
	virtual void init(void) = 0;
	virtual scene_id update(int) = 0;
	virtual void draw(void) = 0;
	virtual scene_id getID(void) const = 0;
protected:
	open_gl_handler& m_gl_handler;
	static int getNewID();
	static int idCounter;
};

