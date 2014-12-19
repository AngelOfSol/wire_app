#pragma once
#include "scene_id.h"
#include "slice_model.h"
#include "sliced_cylinder.h"
#include "tip_model.h"
#include "segment_reader.h"
#include "model_data.h"
#include "tool_path_model.h"
#include "acr_connection.h"
#include "axis_model.h"
#include <memory>
#include <vector>

class render_scene :
	public scene_id_t<render_scene>
{
public:
	render_scene(open_gl_handler&, const model_data& info);
	~render_scene(void);
	void init(void) override;
	scene_id update(int) override;
	void draw(void) override;
	void update_animation(double change);
	void update_realtime(double change);
	
private:
	bool in_real_time;
	bool m_spinning;
	bool m_centerless;
	acr_connection::ptr m_acr;

	float m_model_rotation;

	struct
	{
		glm::vec3 pos;
		struct
		{
			glm::fquat x_axis;
			glm::fquat y_axis;
		} orient;
	} m_camera;

	struct
	{
		std::vector<command> command_list;
		int current_command;
		float elapsed_timer;
		float prev_rotation;
		float angle_traveled;
		glm::vec2 prev_pos;
		glm::vec2 dress_wheel_pos;

		std::vector<glm::vec2> path;
	} m_cmd;

	double m_to_clipper_scalar;


	std::unique_ptr<tip_model> m_tip;

	std::unique_ptr<sliced_cylinder> m_grinded_wire;

	std::unique_ptr<sliced_cylinder> m_prerendered_grinded_wire;

	std::unique_ptr<sliced_cylinder> m_wheel_model;

	std::unique_ptr<slice> m_dress_wheel;

	std::vector<glm::vec2> m_tool_path;

	const model_data& m_input_info;

	tool_path_model m_tool_path_model;

	double units_factor;
	double scale_factor;

	axis_model m_axes;
};

