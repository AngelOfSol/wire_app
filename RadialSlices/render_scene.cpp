#include "render_scene.h"
#include "open_gl_handler.h"
#include "state_gl.h"
#include "segment_reader.h"
#include "Cylinder.h"
#include "make_poly.h"
#include <iostream>
#include "angle.h"
#include "split_string.h"

render_scene::render_scene(open_gl_handler& handle, const model_data& info)
	: scene_id_t(handle)
	, m_input_info(info)
	, units_factor(info.unit_to_inches)
	, m_to_clipper_scalar(1000000)
	, m_tool_path_model(m_tool_path)
{
}


render_scene::~render_scene()
{
}

void render_scene::init(void)
{


	auto part_info = load_commands(this->m_input_info.model_file_name);

	this->m_spinning = true;

	this->units_factor = 1;
	if (!part_info.inches)
		this->units_factor = 25.4;
	this->scale_factor = 0.02 / (part_info.diameter / this->units_factor) * 14;

	const int resolution = 360;
	this->m_grinded_wire = std::make_unique<sliced_cylinder>(glm::vec2{ part_info.length, part_info.diameter }
	, resolution);
	this->m_prerendered_grinded_wire = std::make_unique<sliced_cylinder>(glm::vec2{ part_info.length, part_info.diameter }
	, resolution);

	this->m_camera.pos = glm::vec3(part_info.length * this->scale_factor, 0, -5);

	this->m_tip = std::make_unique<tip_model>();

	this->m_cmd.command_list = std::move(part_info.cmds);

#pragma region is_negative

	for (auto cmd : this->m_cmd.command_list)
	{
		glm::vec2 target(1, 1);
		switch (cmd.active)
		{
		case command::MOVE:
			
			if (cmd.move.y)
			{
				target.y = float(cmd.move.y_move);
			}

			break;
		case command::PLUNGE:
			target = glm::vec2{ this->m_cmd.prev_pos.x, cmd.plunge.plunge_diameter };
			break;
		case command::TAPER:
			target = glm::vec2(cmd.taper.end_length, cmd.taper.end_diameter);
			break;
		case command::POINT_CIRCLE:
			target = glm::vec2{ cmd.point_circle.linear_target_point, cmd.point_circle.ram_target_point / 2 };
			break;
		default:
			break;
		}
		if (target.y < 0)
			throw std::exception("One of your segments have a negative diameter.  This is not supported at this time.");//this->m_gl_handler.close();
	}

#pragma endregion


#pragma region recipe

	if (part_info.recipe)
	{

#pragma region dress_wheel_recipe
		std::string prefix = std::string(R"(C:\Dress Recipes\)");
		auto model_file_name = prefix + part_info.recipe_name;

		auto dress_info = loadMoves(model_file_name);

		auto dress_scale = 1;

		if (part_info.inches && !dress_info.inches)
			dress_scale = 1 / 25.4;
		if (!part_info.inches && dress_info.inches)
			dress_scale = 25.4;

		auto dress_scale_rat = int_frac(dress_scale);

		auto radius = dress_info.diameter / 2.0 * dress_scale;
		auto length = dress_info.length * dress_scale;
		auto x_start = double(dress_info.x_start) * dress_scale;
		auto z_start = double(dress_info.z_start) * dress_scale;

		this->m_dress_wheel = std::make_unique<slice>(glm::vec2{ length, radius });
		Cylinder temp;
		temp.reference().y = radius;

		temp.reference().x = 0;
		if (dress_info.exit)
		{
			temp.reference().x = length;;
		}

		temp.baseLine() = this->m_input_info.config.as<int_frac>("hole-radius") * dress_scale_rat;
		temp.initialSize() = vec2d(length, radius);
		temp.initialTurtle() = vec2d(z_start, x_start);
		temp.initialTurtle() += temp.reference();

		temp.turtleShape() = make_iso_triangle(vec2d(this->m_input_info.config.as<int_frac>("triangle-tip-width") * dress_scale_rat, this->m_input_info.config.as<int_frac>("triangle-tip-height") * dress_scale_rat));

		temp.buildInitial();

		temp.initialSize() = vec2d(length, radius);// radius

		temp.buildInitial();

		for (auto m : dress_info.moves)
		{
			m.speed *= dress_scale_rat;
			m.center *= dress_scale_rat;
			m.end *= dress_scale_rat;
			m.start *= dress_scale_rat;
			//temp.pushMove(m, int_frac(1));
		}

		std::vector<glm::vec2> point_list;
		std::vector<glm::vec2> invert_point_list;

		for (auto v : temp.vertexes())
		{
			point_list.push_back(v);
			v.y *= -1;
			v.y += radius;
			invert_point_list.push_back(v);
		}

		slice o_pie({ length, radius }, point_list);

		this->m_wheel_model = std::make_unique<sliced_cylinder>(o_pie, 4000);

		this->m_dress_wheel = std::make_unique<slice>(glm::vec2{ length, radius }, invert_point_list);

#pragma endregion

	}
	else
	{

#pragma region dress_wheel_parameter
		std::vector<glm::vec2> data;
		std::vector<glm::vec2> inverse_data;

		auto radius = 6 * this->units_factor;

		bool pull = true;

		inverse_data.push_back({ 0, radius });
		inverse_data.push_back({ 0, 0 });
		inverse_data.push_back({ part_info.wheel_data.wheel_width - part_info.wheel_data.wheel_funnel_width, 0 });
		inverse_data.push_back({ part_info.wheel_data.wheel_width, part_info.wheel_data.wheel_depth });
		inverse_data.push_back({ part_info.wheel_data.wheel_width, radius });

		for (auto& v : inverse_data)
		{
			if (pull)
				v.x *= -1;
			auto v_copy = v;
			v_copy.y *= -1;
			v_copy.y += radius;
			data.push_back(v_copy);
		}

		slice o_pie({ part_info.wheel_data.wheel_width, radius }, data);

		this->m_wheel_model = std::make_unique<sliced_cylinder>(o_pie, 4000);
		if (pull)
			part_info.wheel_data.wheel_width *= -1;
		this->m_dress_wheel = std::make_unique<slice>(glm::vec2{ part_info.wheel_data.wheel_width, radius }, inverse_data);

#pragma endregion

	}

#pragma endregion

	this->m_model_rotation = 0;
	this->m_cmd.current_command = 0;
	this->m_cmd.angle_traveled = 0;

	this->m_tip->pos = { 0, 0.1 * this->units_factor, 0 };
	this->m_cmd.dress_wheel_pos = this->m_cmd.prev_pos = { this->m_tip->pos.x, this->m_tip->pos.y };

	this->m_grinded_wire->push();
	this->m_prerendered_grinded_wire->push();


#pragma region pre_rendered_wheel
	
	while (this->m_cmd.current_command < this->m_cmd.command_list.size())
	{

		auto started_at = this->m_cmd.current_command;
		auto old_pos = this->m_cmd.prev_pos;
#pragma region tool tip fsm
		if (this->m_cmd.current_command < this->m_cmd.command_list.size())
		{

			auto& cmd = this->m_cmd.command_list[this->m_cmd.current_command]; 
			auto change = 1.0;
			if (cmd.active == command::POINT_CIRCLE)
				change = 0.01667;

			auto change_f = static_cast<float>(change)* 1;
			glm::vec2 target, vec_change(0, 0), new_pos;
			switch (cmd.active)
			{
			case command::MOVE:
				if (cmd.move.x)
				{
					if (cmd.move.absolute)
					{
						target.x = float(cmd.move.x_move);
					}
					else
					{
						target.x = float(this->m_cmd.prev_pos.x + cmd.move.x_move);
					}
					vec_change.x = float(cmd.move.x_speed * sign(target.x - this->m_cmd.prev_pos.x));
				}
				else
				{
					target.x = this->m_cmd.prev_pos.x;
				}
				if (cmd.move.y)
				{
					target.y = float(cmd.move.y_move);
					vec_change.y = float(cmd.move.y_speed * sign(target.y - this->m_cmd.prev_pos.y));
				}
				else
				{
					target.y = this->m_cmd.prev_pos.y;
				}


				vec_change = change_f * vec_change;
				new_pos = vec_change + this->m_cmd.dress_wheel_pos;

				if (cmd.move.rotation)
				{
					this->m_model_rotation += cmd.move.rotation_speed * change_f * two_pi;
					this->m_cmd.angle_traveled += cmd.move.rotation_speed * change_f * two_pi;


					auto angle_dist = cmd.move.rotation_target - this->m_cmd.prev_rotation;
					if (sign(cmd.move.rotation_speed) == -1)
						angle_dist *= -1;

					angle_dist = normalize_angle(angle_dist);

					if (this->m_cmd.angle_traveled >= angle_dist)
					{
						this->m_cmd.prev_rotation = this->m_model_rotation = cmd.move.rotation_target;
						this->m_cmd.current_command++;
						this->m_cmd.angle_traveled = 0;
					}
				}
				else if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
				{
					this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
					this->m_cmd.current_command++;
				}
				else
				{
					this->m_cmd.dress_wheel_pos += vec_change;
				}


				break;
			case command::TIMER:
				this->m_cmd.elapsed_timer += change_f;
				if (this->m_cmd.elapsed_timer >= cmd.timer.time)
				{
					this->m_cmd.elapsed_timer = 0;
					this->m_cmd.current_command++;
				}
				break;
			case command::COLLET_CONTROL:
				// TODO add bool is_spinning_really_fast

				this->m_spinning = !cmd.collet_control.collet_stop;

				this->m_cmd.current_command++;
				break;
			case command::PLUNGE:
				target = glm::vec2{ this->m_cmd.prev_pos.x, cmd.plunge.plunge_diameter };
				vec_change = change_f * glm::vec2{ 0, cmd.plunge.pull_speed };
				vec_change.y *= sign(target.y - this->m_cmd.prev_pos.y);
				new_pos = vec_change + this->m_cmd.dress_wheel_pos;
				// this lets us know if we've completed or overshot the target point 
				if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
				{
					this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
					this->m_cmd.current_command++;
				}
				else
				{
					this->m_cmd.dress_wheel_pos += vec_change;
				}
				break;
			case command::STRAIGHT:
				target = glm::vec2{ cmd.straight.end_length, this->m_cmd.prev_pos.y };
				vec_change = change_f * glm::vec2{ cmd.straight.pull_speed, 0 };
				if (!cmd.straight.absolute)
					target.x += this->m_cmd.prev_pos.x;
				vec_change.x *= sign(target.x - this->m_cmd.prev_pos.x);
				new_pos = vec_change + this->m_cmd.dress_wheel_pos;
				// this lets us know if we've completed or overshot the target point 
				if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
				{
					this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
					this->m_cmd.current_command++;
				}
				else
				{
					this->m_cmd.dress_wheel_pos += vec_change;
				}
				break;
			case command::TAPER:
				target = glm::vec2(cmd.taper.end_length, cmd.taper.end_diameter);
				if (!cmd.taper.absolute)
					target.x += this->m_cmd.prev_pos.x;
				vec_change = change_f * float(cmd.taper.pull_speed) * glm::normalize(target - this->m_cmd.prev_pos);
				vec_change.x = abs(vec_change.x) * sign(target.x - this->m_cmd.prev_pos.x);
				vec_change.y = abs(vec_change.y) *sign(target.y - this->m_cmd.prev_pos.y);
				new_pos = vec_change + this->m_cmd.dress_wheel_pos;
				// this lets us know if we've completed or overshot the target point
				if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
				{
					this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
					this->m_cmd.current_command++;
				}
				else
				{
					this->m_cmd.dress_wheel_pos += vec_change;
				}
				break;
			case command::PART_INDEX:
				target = this->m_cmd.prev_pos + glm::vec2{ cmd.part_index.part_length, 0 };
				vec_change = change_f * glm::vec2{ cmd.part_index.move_in_velocity, 0 };
				vec_change.x *= sign(target.x - this->m_cmd.prev_pos.x);
				new_pos = vec_change + this->m_cmd.dress_wheel_pos;
				// this lets us know if we've completed or overshot the target point 
				if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
				{
					this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
					this->m_cmd.current_command++;
				}
				else
				{
					this->m_cmd.dress_wheel_pos += vec_change;
				}
				break;
			case command::POINT_CIRCLE:
				// ram is y
				// linear is x
			{

				auto circle_center = glm::vec2{ cmd.point_circle.linear_center_point, cmd.point_circle.ram_center_point };

				auto start_point = glm::vec2{ cmd.point_circle.start_position, cmd.point_circle.start_diameter / 2 };

				if (this->m_cmd.angle_traveled == 0)
					this->m_cmd.dress_wheel_pos = start_point;


				target = glm::vec2{ cmd.point_circle.linear_target_point, cmd.point_circle.ram_target_point / 2 };

				auto radius = glm::length(start_point - circle_center);

				auto theta_second = cmd.point_circle.pull_speed / radius;
				theta_second = abs(theta_second);
				if (!cmd.point_circle.clockwise)
					theta_second *= -1;

				auto start_vec = start_point - circle_center;
				auto end_vec = target - circle_center;

				auto angle_diff = atan2(end_vec.y, end_vec.x) - atan2(start_vec.y, start_vec.x);
				if (sign(theta_second) == -1)
					angle_diff *= -1;
				angle_diff = normalize_angle(angle_diff);

				auto delta_theta = theta_second * change_f;
				this->m_cmd.angle_traveled += delta_theta;

				auto current_vec = this->m_cmd.dress_wheel_pos - circle_center;
				auto current_angle = atan2(current_vec.y, current_vec.x);

				current_angle += delta_theta;

				new_pos = glm::vec2{ radius * cos(current_angle), radius * sin(current_angle) } +circle_center;

				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos;
				old_pos = this->m_cmd.prev_pos;
				this->m_cmd.dress_wheel_pos = new_pos;

				this->m_prerendered_grinded_wire->push();


				if (this->m_cmd.angle_traveled >= angle_diff)
				{
					this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
					this->m_cmd.angle_traveled = 0;
					this->m_cmd.current_command++;
				}
			}

				break;
			default:
				this->m_cmd.current_command++;
				break;
			}
			this->m_tip->pos = glm::vec3(this->m_cmd.dress_wheel_pos, 0);
			//std::cout << " #" << (this->m_cmd.current_command + 1) << " @ " << this->m_tip.pos.x << ", " << this->m_tip.pos.y << std::endl;
			//std::cout << " #" << (this->m_cmd.current_command + 1) << " from (" << this->m_cmd.prev_pos.x / unit_per_inch << ", " << this->m_cmd.prev_pos.y / unit_per_inch << ") to (" << target.x / unit_per_inch << ", " << target.y / unit_per_inch << ")" << std::endl;
		}
#pragma endregion

#pragma region grinding_wire
		auto diff = (this->m_cmd.dress_wheel_pos - old_pos);
		if (abs(diff.x) >= 0.0001 || abs(diff.y) >= 0.0001)
		{
			this->m_prerendered_grinded_wire->pop();
			this->m_prerendered_grinded_wire->push();

			auto swept = this->m_dress_wheel->sweep({ this->m_to_clipper_scalar, this->m_to_clipper_scalar }, this->m_cmd.dress_wheel_pos, old_pos);

			// resets the state of the cylinder to the last finished move
			if (this->m_spinning)
			{
				this->m_prerendered_grinded_wire->radial_cut({ this->m_to_clipper_scalar, this->m_to_clipper_scalar }, { 0, 0 }, swept, { 1, 1 }, {});
				//this->m_spinning = false;
			}
			else
			{
				this->m_prerendered_grinded_wire->flat_cut({ this->m_to_clipper_scalar, this->m_to_clipper_scalar }, { 0, 0 }, swept, { 1, 1 }, {}, -this->m_model_rotation);//this->m_model_rotation);
			}

			
		}
		if (started_at != this->m_cmd.current_command)
		{
			this->m_prerendered_grinded_wire->push();
		}

#pragma endregion
	}
	this->m_prerendered_grinded_wire->update_models();
#pragma endregion


	this->m_model_rotation = 0;
	this->m_cmd.current_command = 0;
	this->m_cmd.angle_traveled = 0;

	this->m_tip->pos = { 0, 0.1 * this->units_factor, 0 };
	this->m_cmd.dress_wheel_pos = this->m_cmd.prev_pos = { this->m_tip->pos.x, this->m_tip->pos.y };

	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDepthRange(0.0f, 1.0f);
	glClearDepth(1.0f);

	this->in_real_time = this->m_input_info.start_live;

}
scene_id render_scene::update(int elapsed)
{
	auto change = elapsed / 1000.0;

	this->m_model_rotation = normalize_angle(this->m_model_rotation);
	if (this->in_real_time)
		this->update_realtime(change);
	else
		this->update_animation(change);


#pragma region keys

	auto speed_change = static_cast<float>(change)* 3.0f;
	auto rotation_change = static_cast<float>(change)* 1.0f;

	auto rotate = [](glm::vec3 axis, float change)
	{
		// get the change in mouse position

		// convert the change in x to a change in the y axis angle in radians
		float axisRotation = change;

		// 2.0f is just a small scaling value
		axis *= sinf(axisRotation) / 2.0f;
		float scalar = cosf(axisRotation / 2.0f);

		// create a quaternion from the axis and the scalar
		return glm::fquat(scalar, axis.x, axis.y, axis.z);
	};

	if (this->m_gl_handler.keys()['w'])
	{
		this->m_camera.pos.y += speed_change;
	}
	if (this->m_gl_handler.keys()['s'])
	{
		this->m_camera.pos.y -= speed_change;
	}
	if (this->m_gl_handler.keys()['d'])
	{
		this->m_camera.pos.x += speed_change;
	}
	if (this->m_gl_handler.keys()['a'])
	{
		this->m_camera.pos.x -= speed_change;
	}
	if (this->m_gl_handler.keys()['e'])
	{
		this->m_camera.pos.z *= (1 + 0.55 * change);
	}
	if (this->m_gl_handler.keys()['q'])
	{
		this->m_camera.pos.z *= (1 - 0.55 * change);
	}

	if (this->m_gl_handler.keys()['p'])
	{
		this->m_gl_handler.close();
	}

	if (this->m_gl_handler.keys().up)
	{
		this->m_camera.orient.x_axis = glm::normalize(rotate(glm::vec3(1, 0, 0), rotation_change)) * this->m_camera.orient.x_axis;
	}
	if (this->m_gl_handler.keys().down)
	{
		this->m_camera.orient.x_axis = glm::normalize(rotate(glm::vec3(1, 0, 0), -rotation_change)) * this->m_camera.orient.x_axis;
	}

	if (this->m_gl_handler.keys().left)
	{
		this->m_camera.orient.y_axis = glm::normalize(rotate(glm::vec3(0, 1, 0), rotation_change)) * this->m_camera.orient.y_axis;
	}
	if (this->m_gl_handler.keys().right)
	{
		this->m_camera.orient.y_axis = glm::normalize(rotate(glm::vec3(0, 1, 0), -rotation_change)) * this->m_camera.orient.y_axis;
	}

	if (this->m_gl_handler.keys()['i'])
	{
		this->in_real_time = true;
	}
	if (this->m_gl_handler.keys()['o'])
	{
		this->in_real_time = false;
	}
#pragma endregion

	if (this->m_input_info.pipe->check())
	{
		std::string input = this->m_input_info.pipe->get();

		if (input == "close")
			this->m_gl_handler.close();
		if (input.substr(0, 4) == "move")
		{
			input = input.substr(5);
			auto nums = split(input, ' ');
			int x = std::stoi(nums[0]);
			int y = std::stoi(nums[1]);
			int w = std::stoi(nums[2]);
			int h = std::stoi(nums[3]);

			SetWindowPos(this->m_gl_handler.hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
		}

	}
	SetWindowPos(this->m_gl_handler.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);


	this->m_tool_path.push_back(this->m_cmd.dress_wheel_pos);
	this->m_tool_path_model.update();

	return this->getID();
}

void render_scene::update_realtime(double change)
{
	if (this->m_acr == nullptr && this->m_input_info.acr)
	{
		this->m_acr = make_connection("192.168.10.40");
	}
	else if (this->m_acr != nullptr)
	{
		this->m_model_rotation = this->m_acr->getLong(12802) / 4000.0 * two_pi;
		this->m_tip->pos.x = this->m_acr->getLong(12546) / 254000.0 / this->units_factor;
		this->m_tip->pos.y = this->m_acr->getLong(12290) / 254000.0 / this->units_factor;
	}
}

void render_scene::update_animation(double change)
{
	auto started_at = this->m_cmd.current_command;
	auto old_pos = this->m_cmd.prev_pos;
#pragma region tool tip fsm
	if (this->m_cmd.current_command < this->m_cmd.command_list.size())
	{
		auto change_f = static_cast<float>(change)* 1;
		auto& cmd = this->m_cmd.command_list[this->m_cmd.current_command];
		glm::vec2 target, vec_change(0, 0), new_pos;
		switch (cmd.active)
		{
		case command::MOVE:
			if (cmd.move.x)
			{
				if (cmd.move.absolute)
				{
					target.x = float(cmd.move.x_move);
				}
				else
				{
					target.x = float(this->m_cmd.prev_pos.x + cmd.move.x_move);
				}
				vec_change.x = float(cmd.move.x_speed * sign(target.x - this->m_cmd.prev_pos.x));
			}
			else
			{
				target.x = this->m_cmd.prev_pos.x;
			}
			if (cmd.move.y)
			{
				target.y = float(cmd.move.y_move);
				vec_change.y = float(cmd.move.y_speed * sign(target.y - this->m_cmd.prev_pos.y));
			}
			else
			{
				target.y = this->m_cmd.prev_pos.y;
			}


			vec_change = change_f * vec_change;
			new_pos = vec_change + this->m_cmd.dress_wheel_pos;

			if (cmd.move.rotation)
			{
				this->m_model_rotation += cmd.move.rotation_speed * change_f * two_pi;
				this->m_cmd.angle_traveled += cmd.move.rotation_speed * change_f * two_pi;


				auto angle_dist = cmd.move.rotation_target - this->m_cmd.prev_rotation;
				if (sign(cmd.move.rotation_speed) == -1)
					angle_dist *= -1;

				angle_dist = normalize_angle(angle_dist);

				if (this->m_cmd.angle_traveled >= angle_dist)
				{
					this->m_cmd.prev_rotation = this->m_model_rotation = cmd.move.rotation_target;
					this->m_cmd.current_command++;
					this->m_cmd.angle_traveled = 0;
				}
			}
			else if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
			{
				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
				this->m_cmd.current_command++;
			}
			else
			{
				this->m_cmd.dress_wheel_pos += vec_change;
			}


			break;
		case command::TIMER:
			this->m_cmd.elapsed_timer += change_f;
			if (this->m_cmd.elapsed_timer >= cmd.timer.time)
			{
				this->m_cmd.elapsed_timer = 0;
				this->m_cmd.current_command++;
			}
			break;
		case command::COLLET_CONTROL:
			// TODO add bool is_spinning_really_fast

			this->m_spinning = !cmd.collet_control.collet_stop;

			this->m_cmd.current_command++;
			break;
		case command::PLUNGE:
			target = glm::vec2{ this->m_cmd.prev_pos.x, cmd.plunge.plunge_diameter };
			vec_change = change_f * glm::vec2{ 0, cmd.plunge.pull_speed };
			vec_change.y *= sign(target.y - this->m_cmd.prev_pos.y);
			new_pos = vec_change + this->m_cmd.dress_wheel_pos;
			// this lets us know if we've completed or overshot the target point 
			if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
			{
				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
				this->m_cmd.current_command++;
			}
			else
			{
				this->m_cmd.dress_wheel_pos += vec_change;
			}
			break;
		case command::STRAIGHT:
			target = glm::vec2{ cmd.straight.end_length, this->m_cmd.prev_pos.y };
			vec_change = change_f * glm::vec2{ cmd.straight.pull_speed, 0 };
			if (!cmd.straight.absolute)
				target.x += this->m_cmd.prev_pos.x;
			vec_change.x *= sign(target.x - this->m_cmd.prev_pos.x);
			new_pos = vec_change + this->m_cmd.dress_wheel_pos;
			// this lets us know if we've completed or overshot the target point 
			if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
			{
				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
				this->m_cmd.current_command++;
			}
			else
			{
				this->m_cmd.dress_wheel_pos += vec_change;
			}
			break;
		case command::TAPER:
			target = glm::vec2(cmd.taper.end_length, cmd.taper.end_diameter);
			if (!cmd.taper.absolute)
				target.x += this->m_cmd.prev_pos.x;
			vec_change = change_f * float(cmd.taper.pull_speed) * glm::normalize(target - this->m_cmd.prev_pos);
			vec_change.x = abs(vec_change.x) * sign(target.x - this->m_cmd.prev_pos.x);
			vec_change.y = abs(vec_change.y) *sign(target.y - this->m_cmd.prev_pos.y);
			new_pos = vec_change + this->m_cmd.dress_wheel_pos;
			// this lets us know if we've completed or overshot the target point
			if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
			{
				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
				this->m_cmd.current_command++;
			}
			else
			{
				this->m_cmd.dress_wheel_pos += vec_change;
			}
			break;
		case command::PART_INDEX:
			target = this->m_cmd.prev_pos + glm::vec2{ cmd.part_index.part_length, 0 };
			vec_change = change_f * glm::vec2{ cmd.part_index.move_in_velocity, 0 };
			vec_change.x *= sign(target.x - this->m_cmd.prev_pos.x);
			new_pos = vec_change + this->m_cmd.dress_wheel_pos;
			// this lets us know if we've completed or overshot the target point 
			if (glm::dot((this->m_cmd.prev_pos - target), (new_pos - target)) <= 0)
			{
				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
				this->m_cmd.current_command++;
			}
			else
			{
				this->m_cmd.dress_wheel_pos += vec_change;
			}
			break;
		case command::POINT_CIRCLE:
			// ram is y
			// linear is x
		{

			auto circle_center = glm::vec2{ cmd.point_circle.linear_center_point, cmd.point_circle.ram_center_point };

			auto start_point = glm::vec2{ cmd.point_circle.start_position, cmd.point_circle.start_diameter / 2 };

			if (this->m_cmd.angle_traveled == 0)
				this->m_cmd.dress_wheel_pos = start_point;


			target = glm::vec2{ cmd.point_circle.linear_target_point, cmd.point_circle.ram_target_point / 2 };

			auto radius = glm::length(start_point - circle_center);

			auto theta_second = cmd.point_circle.pull_speed / radius;
			theta_second = abs(theta_second);
			if (!cmd.point_circle.clockwise)
				theta_second *= -1;

			auto start_vec = start_point - circle_center;
			auto end_vec = target - circle_center;

			auto angle_diff = atan2(end_vec.y, end_vec.x) - atan2(start_vec.y, start_vec.x);
			if (sign(theta_second) == -1)
				angle_diff *= -1;
			angle_diff = normalize_angle(angle_diff);

			auto delta_theta = theta_second * change_f;
			this->m_cmd.angle_traveled += delta_theta;

			auto current_vec = this->m_cmd.dress_wheel_pos - circle_center;
			auto current_angle = atan2(current_vec.y, current_vec.x);

			current_angle += delta_theta;

			new_pos = glm::vec2{ radius * cos(current_angle), radius * sin(current_angle) } +circle_center;

			this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos;
			old_pos = this->m_cmd.prev_pos;
			this->m_cmd.dress_wheel_pos = new_pos;

			this->m_grinded_wire->push();


			if (this->m_cmd.angle_traveled >= angle_diff)
			{
				this->m_cmd.prev_pos = this->m_cmd.dress_wheel_pos = target;
				this->m_cmd.angle_traveled = 0;
				this->m_cmd.current_command++;
			}
		}

			break;
		default:
			this->m_cmd.current_command++;
			break;
		}
		this->m_tip->pos = glm::vec3(this->m_cmd.dress_wheel_pos, 0);
		//std::cout << " #" << (this->m_cmd.current_command + 1) << " @ " << this->m_tip.pos.x << ", " << this->m_tip.pos.y << std::endl;
		//std::cout << " #" << (this->m_cmd.current_command + 1) << " from (" << this->m_cmd.prev_pos.x / unit_per_inch << ", " << this->m_cmd.prev_pos.y / unit_per_inch << ") to (" << target.x / unit_per_inch << ", " << target.y / unit_per_inch << ")" << std::endl;
	}
#pragma endregion

#pragma region grinding_wire
	auto diff = (this->m_cmd.dress_wheel_pos - old_pos);
	if (abs(diff.x) >= 0.0001 || abs(diff.y) >= 0.0001)
	{
		this->m_grinded_wire->pop();
		this->m_grinded_wire->push();

		auto swept = this->m_dress_wheel->sweep({ this->m_to_clipper_scalar, this->m_to_clipper_scalar }, this->m_cmd.dress_wheel_pos, old_pos);

		// resets the state of the cylinder to the last finished move
		if (this->m_spinning)
		{
			this->m_grinded_wire->radial_cut({ this->m_to_clipper_scalar, this->m_to_clipper_scalar }, { 0, 0 }, swept, { 1, 1 }, {});
			//this->m_spinning = false;
		}
		else
		{
			this->m_grinded_wire->flat_cut({ this->m_to_clipper_scalar, this->m_to_clipper_scalar }, { 0, 0 }, swept, { 1, 1 }, {}, -this->m_model_rotation);//this->m_model_rotation);
		}

		this->m_grinded_wire->update_models();
	}
	if (started_at != this->m_cmd.current_command)
	{
		this->m_grinded_wire->push();
	}

#pragma endregion

}

void render_scene::draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	auto total_orient = this->m_camera.orient.x_axis * this->m_camera.orient.y_axis;

	// cast the quarternion back to a rotation matrix
	auto rotateMat = glm::mat4_cast(total_orient);


	auto perspective = glm::perspective(45.0f, this->m_gl_handler.window_size().x / this->m_gl_handler.window_size().y, 0.01f, 1000.0f);
	auto view = glm::mat4()
		* glm::translate(glm::vec3(0, 0, this->m_camera.pos.z))
		* rotateMat
		* glm::translate(glm::vec3(this->m_camera.pos.x, this->m_camera.pos.y, 0))
		* glm::translate(glm::vec3(-this->m_cmd.dress_wheel_pos.x - this->m_dress_wheel->initial_dimensions.x / 2, 0, 0) / static_cast<float>(this->units_factor))
		;

	gl_state_on<GL_DEPTH_TEST> depth;


	auto scale_mat = glm::scale(glm::vec3{ 1, this->scale_factor, this->scale_factor } *1.0f / static_cast<float>(this->units_factor));
	if (this->in_real_time)
		this->m_prerendered_grinded_wire->draw(perspective, view, scale_mat * glm::rotate(this->m_model_rotation, glm::vec3{ 1, 0, 0 }));
	else
		this->m_grinded_wire->draw(perspective, view, scale_mat * glm::rotate(this->m_model_rotation, glm::vec3{1, 0, 0}));

	uniform(slice_model::shader(), "color", glm::vec4(0.5f, 0.5f, 0.3f, 1.0f));

	this->m_wheel_model->draw(perspective, view, scale_mat * glm::translate(glm::vec3{ this->m_tip->pos.x, 0, -this->m_dress_wheel->initial_dimensions.y + -this->m_tip->pos.y }));

	uniform(slice_model::shader(), "color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	auto axis_scale = glm::scale(glm::vec3{ 2, 2, 2 });
	auto axis_transform = glm::translate(glm::vec3(0, 0, this->m_camera.pos.z))
		* rotateMat;

	glLineWidth(5);
	this->m_axes.draw(perspective, axis_transform, axis_scale);
	gl_state_off<GL_DEPTH_TEST> no_depth;
	glLineWidth(2);
	this->m_axes.draw(perspective, axis_transform, axis_scale);
	// to remove

	//this->m_tip->draw(perspective, view, scale_mat * glm::translate(this->m_tip->pos));

	//this->m_tool_path_model.draw(glm::ortho(0.4f, 8.6f, -0.1f, 0.05f));

}
