#pragma once
#include "scene.h"
template <class T>
class scene_id_t :
	public scene
{
public:
	scene_id_t(open_gl_handler& parent) :scene(parent)
	{
	}
	~scene_id_t(void)
	{
	}
	scene_id getID(void) const override
	{
		return scene_id_t::id;
	}
	static const scene_id id;
private:

};
template<class T>
const scene_id scene_id_t<T>::id = scene::getNewID();
