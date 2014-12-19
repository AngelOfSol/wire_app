#pragma once
#include "typedefs.h"
#include <vector>
#include "Vec2D.h"
struct missing_data
{
	std::string data_name;
	std::string location_name;
	missing_data(std::string s, std::string l = "Start") : data_name(s), location_name(l)
	{

	}
};
enum class MoveType
{
	NONE = 0,
	STRAIGHT = 1,
	TAPER = 2,
	PLUNGE = 6,
	MOVE = 9,
	CIRCLE = 14,
	INVALID = -1
};

struct Move
{
	
	MoveType id;
	bool absolute;
	bool returnToStartDiameter;

	vec2d start;
	vec2d end;
	vec2d speed;

	bool radiusAffected;
	bool positionAffected;

	vec2d center;


};

struct move_data
{
	double diameter;
	double length;
	int_frac x_start;
	int_frac z_start;
	bool exit;
	bool inches;

	std::vector<Move> moves;
};

struct command
{
	enum direction
	{
		PUSH = 0,
		PULL = 1
	};
	union
	{
		struct
		{
			bool x;
			double x_move; // 1
			double x_speed; // 2

			bool y;

			double y_move; // 3
			double y_speed; // 4

			bool rotation;

			double rotation_target; // 5
			double rotation_speed; // 6
			
			bool used_for_total_length_adjust; // 8
			bool absolute; // 9
		} move;

		struct
		{
			bool collet_position_for_grip; // 3
			double part_length; // 4
			double move_in_velocity; // 5
			double move_out_velocity; // 6
			double collet_speed; // 8
			direction grind_direction; // 9
		} part_index;

		struct
		{
			bool clockwise; // 1
			double pull_speed; // 2
			double start_diameter; // 3
			double start_position; // 4
			double ram_target_point; // 5
			double ram_center_point; // 6
			double linear_target_point; // 7
			double linear_center_point; // 8
			bool tool_radius_exists; // 9
			union
			{
				double tool_radius;
			};
		} point_circle;

		struct
		{
			bool collet_stop; // 1
			double collet_rpm; // 2
			bool direction_cw; // 3
			bool direction_ccw; // 4
		} collet_control;

		struct
		{
			double pull_speed; // 1
			double start_diameter; // 2
			double start_length; // 3
			double plunge_diameter; // 4
			bool return_to_start; // 5
		} plunge;

		struct
		{
			double pull_speed; // 1
			double ground_diameter; // 2
			double start_length; // 3
			double end_length; // 4
			bool flat_number; // 8
			bool absolute; // 9
		} straight;

		struct
		{
			double time; // 1
		} timer;

		struct
		{
			double pull_speed; // 1
			double start_diameter; // 2
			double start_length; // 3
			double end_diameter; // 4
			double end_length; // 5
			double start_and_end_length_number; // 8
			bool absolute; // 9
		} taper;
		struct
		{

		} io_control;
	};
	enum type
	{
		INVALID = 0
		, MOVE = 9
		, PART_INDEX = 13
		, POINT_CIRCLE = 12
		, COLLET_CONTROL = 11
		, PLUNGE = 6
		, STRAIGHT = 1
		, TIMER = 10
		, TAPER = 2
		, IO_CONTROL = 8

	} active;
};

struct command_data
{
	double diameter;
	double length;

	bool bushing;
	bool recipe;
	std::string recipe_name;
	struct
	{
		double wheel_width;
		double wheel_depth;
		double wheel_funnel_width;
	} wheel_data;

	bool inches;
	std::vector<command> cmds;
};

move_data loadMoves(std::string);
command_data load_commands(std::string);