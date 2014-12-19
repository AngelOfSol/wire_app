#include "angle.h"
#include "segment_reader.h"
#include <fstream>
#include <functional>
#include <boost\optional.hpp>
#include "regex_ext.h"
#include "debug.h"

boost::optional<double> try_parse(const std::string& ref)
{
	try
	{
		return std::stod(ref);
	}
	catch (std::invalid_argument e)
	{
		return boost::optional<double>();
	}
}

template <typename T>
void fillOut(std::string data, const boost::regex& regex, T& target, const std::function<T(const boost::smatch&)>& operate, T defaultValue)
{
	boost::smatch results;
	if (boost::regex_search(data, results, regex))
	{
		target = operate(results);
	}
	else
	{
		target = defaultValue;
	}
}



std::vector<std::vector<std::string>> load_table_data(std::string file_data)
{
	std::vector<std::vector<std::string>> data;

	boost::smatch results;

	boost::regex tableRow("<tr[^>]*>(.+?)</tr>");
	boost::regex tableCell("<td[^>]*>(.+?)</td>");
	boost::regex colspan(R"reg(colspan="([\d]+)")reg");
	std::string rowData;
	std::string cellData;
	auto tableDataBegin = file_data.cbegin();
	auto tableDataEnd = file_data.cend();

	while (boost::regex_search(tableDataBegin, tableDataEnd, results, tableRow))
	{
		rowData = results[1];
		tableDataBegin = results[0].second;
		data.push_back(std::vector<std::string>());
		auto rowDataBegin = rowData.cbegin();
		auto rowDataEnd = rowData.cend();
		while (boost::regex_search(rowDataBegin, rowDataEnd, results, tableCell))
		{
			if (results[1] != "&nbsp;")
			{
				data.back().push_back(results[1].str());
			}
			else
			{
				data.back().push_back("");
			}
			if (boost::regex_search(results[0].first, results[0].second, results, colspan))
			{
				int extra_columns = std::stoi(results[1].str()) - 1;
				for (int i = 0; i < extra_columns; i++)
				{
					data.back().push_back("");
				}
			}
			rowDataBegin = results[0].second;

		}
	}


	return data;
}


move_data loadMoves(std::string fileName)
{
	move_data ret;
	std::vector<std::vector<std::string>> data;

	std::ifstream inputFile(fileName);

	std::string fileString = "";
	std::string inputLine;

	boost::regex testforTable("table");

	bool inTable = false;
	while (inputFile.good())
	{
		std::getline(inputFile, inputLine);
		if (boost::regex_search(inputLine, testforTable))
		{
			inTable = !inTable;
		}
		if (inTable)
		{
			fileString += inputLine;
		}
	}

	boost::smatch results;

	data = load_table_data(fileString);

	std::string floatingPointNumber = "(-?[\\d]+\\.?[\\d]*)";
	boost::regex typeIDRegex("(Straight|Taper|Plunge|Move Control|Point Circle) = ([\\d]{1,2})");
	boost::regex speedRegex("(X Move Speed|Pull Speed|Z Move Speed) = " + floatingPointNumber);
	boost::regex absoluteMoveRegex("Absolute Move for Length = (0|1)");
	boost::regex returnToStartRegex("Return to Start Diameter = (0|1)");
	boost::regex startXRegex("(Start Length Position|Start Length Position \\(Z\\)) = " + floatingPointNumber);
	boost::regex endXRegex("(Z Position|End Length Position|Length Absolute Target Point|Linear Target Point \\(Z\\)) = " + floatingPointNumber);
	boost::regex startYRegex("(Start Diameter|Start Diameter \\(X\\)|Ground Diameter) = " + floatingPointNumber);
	boost::regex endYRegex("(X Position|End Diameter of Taper|Plunge Diameter|Diameter Absolute Target Point|Diameter Target Point \\(X\\)) = " + floatingPointNumber);
	boost::regex centerXRegex("(Length Absolute Center Point|Linear Center Point \\(Z\\)) = " + floatingPointNumber);
	boost::regex centerYRegex("(Diameter Absolute Center Point|Diameter Center Point \\(X\\)) = " + floatingPointNumber);
	boost::regex isNumber("\\A" + floatingPointNumber + "\\z");
	bool done = false;

	auto trim = [](std::string val)
	{
		std::string ret = "";
		bool onSpace = false;
		for (char c : val)
		{
			if (c >= 0 && c <= 255)
			{

				if (std::isspace(c))
				{
					if (!onSpace)
					{
						ret += ' ';
						onSpace = true;
					}
				}
				else
				{
					onSpace = false;
					ret += c;
				}
			}
		}
		return ret;
	};
	struct
	{
		double diameter;
		double length;
		int_frac x_start;
		int_frac z_start;
		bool exit;
		bool inches;
	} other_data;


	auto result = try_parse(data[1][3]);
	if (result)
		ret.diameter = result.get();
	else
		throw missing_data("Diameter");

	result = try_parse(data[2][3]);
	if (result)
		ret.length = result.get();
	else
		throw missing_data("Length");

	result = try_parse(data[1][6]);
	if (result)
		ret.x_start = result.get();
	else
		throw missing_data("X Tool Home");

	result = try_parse(data[2][6]);
	if (result)
		ret.z_start = result.get();
	else
		throw missing_data("Z Tool Home");


	/*ret.diameter = std::stod(data[1][3]);
	ret.length = std::stod(data[2][3]);
	ret.x_start = stor <base_int>(data[1][6]);
	ret.z_start = stor <base_int>(data[2][6]);*/
	ret.exit = data[3][3] == "Exit" || data[3][3] == "exit";
	ret.inches = "mm" != data[12][10] && "MM" != data[12][10];
	ret.exit = data[3][3] == "Exit" || data[3][3] == "exit";
	ret.inches = "mm" != data[12][10] && "MM" != data[12][10];

	for (unsigned int i = 0; i < data.size() && !done; i++)
	{
		auto& row = data[i];
		for (unsigned int j = 0; j < row.size() && !done; j++)
		{
			auto& cell = data[i][j];
			if (matches(cell, "Segment[^#]+#[\\d]+"))
			{
				std::string segmentData = "";
				std::string tempData = "";
				bool empty = true;
				for (unsigned int y = i + 1; y < i + 11; y++)
				{
					empty = empty && data[y][j] == "";

					try
					{
						tempData += trim(data[y][j]);
						tempData += " = ";
						tempData += data[y][j + 1];
						tempData += ";\n";
						segmentData += tempData;
					}
					catch (...)
					{

					}
				}
				done = empty;
				if (!empty)
				{
					bool absoluteTest = false;

					ret.moves.push_back(Move());

					auto& lastSegment = ret.moves.back();

					//(data, regex, parse [T actOn(std::string)], default);


					auto boolParse = [](const boost::smatch& results) { return "1" == results[1];  };
					auto intParse = [](const boost::smatch& results) { return std::stoi(results[2]); };
					auto moveParse = [](const boost::smatch& results) { return (MoveType)std::stoi(results[2]); };
					auto doubleParse = [](const boost::smatch& results) { return stor<base_int>(results[2]); };

					auto fillBool = [&segmentData, &boolParse](bool& target, bool default, const boost::regex& regex)
					{
						fillOut<bool>(
							segmentData
							, regex
							, target
							, boolParse
							, default
							);
					};
					auto fillInt = [&segmentData, &intParse](int_frac& target, int default, const boost::regex& regex)
					{
						int ref = 0;
						fillOut<int>(
							segmentData
							, regex
							, ref
							, intParse
							, default
							);
						target = ref;
					};
					auto fillDouble = [&segmentData, &doubleParse](int_frac& target, int_frac default, const boost::regex& regex)
					{
						fillOut<int_frac>(
							segmentData
							, regex
							, target
							, doubleParse
							, default
							);
					};


					fillOut<MoveType>(
						segmentData
						, typeIDRegex
						, lastSegment.id
						, moveParse
						, MoveType::INVALID
						);


					fillBool(lastSegment.returnToStartDiameter, false, returnToStartRegex);
					fillBool(lastSegment.absolute, true, absoluteMoveRegex);


					try
					{
						fillDouble(lastSegment.start.x, 0, startXRegex);
						fillDouble(lastSegment.start.y, 0, startYRegex);

						fillDouble(lastSegment.end.x, 0, endXRegex);
						fillDouble(lastSegment.end.y, 0, endYRegex);

						fillDouble(lastSegment.center.x, 0, centerXRegex);
						fillDouble(lastSegment.center.y, 0, centerYRegex);
					}
					catch (std::invalid_argument e)
					{
						throw missing_data("NONE", cell);
					}

					lastSegment.positionAffected = boost::regex_search(segmentData, results, endXRegex);

					lastSegment.radiusAffected = boost::regex_search(segmentData, results, endYRegex);

					auto begin = segmentData.cbegin();
					auto end = segmentData.cend();
					while (boost::regex_search(begin, end, results, speedRegex))
					{
						if (results[1] == "Z Move Speed")
						{
							lastSegment.speed.x = stor<base_int>(results[2]);
						}
						else if (results[1] == "Pull Speed")
						{
							switch (lastSegment.id)
							{
							case MoveType::STRAIGHT:
							case MoveType::TAPER:
								lastSegment.speed.x = stor<base_int>(results[2]);
								break;
							case MoveType::CIRCLE:
							case MoveType::PLUNGE:
								lastSegment.speed.y = stor<base_int>(results[2]);
								break;
							default:
								break;
							}

						}
						else if (results[1] == "X Move Speed")
						{
							lastSegment.speed.y = stor<base_int>(results[2]);
						}

						begin = results[0].second;
					}
				}
			}
		}
	}

	return ret;
}

command_data load_commands(std::string fileName)
{
	command_data ret;

	std::vector<std::vector<std::string>> data;

	std::ifstream inputFile(fileName);

	std::string fileString = "";
	std::string inputLine;

	boost::regex testforTable("table");

	bool inTable = false;
	while (inputFile.good())
	{
		std::getline(inputFile, inputLine);
		if (boost::regex_search(inputLine, testforTable))
		{
			inTable = !inTable;
		}
		if (inTable)
		{
			fileString += inputLine;
		}
	}

	boost::smatch results;

	data = load_table_data(fileString);

	bool done = false;

	auto trim = [](std::string val)
	{
		std::string ret = "";
		bool onSpace = false;
		for (char c : val)
		{
			if (c >= 0 && c <= 255)
			{

				if (std::isspace(c))
				{
					if (!onSpace)
					{
						ret += ' ';
						onSpace = true;
					}
				}
				else
				{
					onSpace = false;
					ret += c;
				}
			}
		}
		return ret;
	};

	auto result = try_parse(data[3][11]);
	if (result)
		ret.diameter = result.get();
	else
		throw missing_data("Wire Diameter");

	result = try_parse(data[2][3]);
	if (result)
		ret.length = result.get();
	else
		throw missing_data("Wire Length");

	ret.inches = "mm" != data[12][10] && "MM" != data[12][10];
	ret.bushing = "x" == data[12][10];

	ret.recipe_name = data[1][9];
	if (ret.recipe_name == "")
	{
		ret.recipe = false;

		result = try_parse(data[5][11]);
		if (result)
			ret.wheel_data.wheel_width = result.get();
		else
			throw missing_data("Wheel Width");

		result = try_parse(data[6][11]);
		if (result)
			ret.wheel_data.wheel_funnel_width = result.get();
		else
			throw missing_data("Wheel Funnel Width");

		result = try_parse(data[7][11]);
		if (result)
			ret.wheel_data.wheel_depth = result.get();
		else
			throw missing_data("Wheel Funnel Depth");


	}

	for (unsigned int i = 0; i < data.size() && !done; i++)
	{
		auto& row = data[i];
		for (unsigned int j = 0; j < row.size() && !done; j++)
		{
			auto& cell = data[i][j];
			if (matches(cell, "Segment[^#]+#[\\d]+"))
			{
				if (!std::isdigit(data[i + 1][j + 1][0]))
				{
					done = true;
				}
				else
				{
					try
					{
						command new_command;
						new_command.active = command::type(std::stoi(data[i + 1][j + 1]));
						auto get = [&data, i, j](int row)
						{
							return data[i + 1 + row][j + 1];
						};
						switch (new_command.active)
						{
						case command::MOVE:
							try
							{
								new_command.move.y_move = std::stod(get(1));
								new_command.move.y_speed = std::stod(get(2));
								new_command.move.y = true;
							}
							catch (std::invalid_argument i)
							{
								new_command.move.y = false;
							}
							try
							{
								new_command.move.x_move = std::stod(get(3));
								new_command.move.x_speed = std::stod(get(4));
								new_command.move.x = true;
							}
							catch (std::invalid_argument i)
							{
								new_command.move.x = false;
							}
							try
							{
								new_command.move.rotation_target = std::stod(get(5)) * pi / 180.0;
								new_command.move.rotation_speed = std::stod(get(6)) * pi / 180.0;
								new_command.move.rotation = true;
							}
							catch (std::invalid_argument i)
							{
								new_command.move.rotation = false;
							}
							new_command.move.used_for_total_length_adjust = get(8) == "1";
							new_command.move.absolute = get(9) == "1";
							break;
						case command::PART_INDEX:
							new_command.part_index.collet_position_for_grip = get(3) == "1";
							new_command.part_index.part_length = std::stod(get(4));
							new_command.part_index.move_in_velocity = std::stod(get(5));
							new_command.part_index.move_out_velocity = std::stod(get(6));
							new_command.part_index.collet_speed = std::stod(get(8));
							new_command.part_index.grind_direction = command::direction(std::stoi(get(9)));
							break;
						case command::POINT_CIRCLE:
							new_command.point_circle.clockwise = get(1) == "1";
							new_command.point_circle.pull_speed = std::stod(get(2));
							new_command.point_circle.start_diameter = std::stod(get(3));
							new_command.point_circle.start_position = std::stod(get(4));
							new_command.point_circle.ram_target_point = std::stod(get(5));
							new_command.point_circle.ram_center_point = std::stod(get(6));
							new_command.point_circle.linear_target_point = std::stod(get(7));
							new_command.point_circle.linear_center_point = std::stod(get(8));
							try
							{
								new_command.point_circle.tool_radius = std::stod(get(9));
								new_command.point_circle.tool_radius_exists = true;
							}
							catch (std::invalid_argument i)
							{
								new_command.point_circle.tool_radius_exists = false;
							}
							break;
						case command::COLLET_CONTROL:
							new_command.collet_control.collet_stop = get(1) == "1";
							new_command.collet_control.collet_rpm = std::stod(get(2));
							new_command.collet_control.direction_cw = get(3) == "1";
							new_command.collet_control.direction_ccw = get(4) == "1";
							break;
						case command::PLUNGE:
							new_command.plunge.pull_speed = std::stod(get(1));
							new_command.plunge.start_diameter = std::stod(get(2));
							new_command.plunge.start_length = std::stod(get(3));
							new_command.plunge.plunge_diameter = std::stod(get(4));
							new_command.plunge.return_to_start = get(5) == "1";
							break;
						case command::STRAIGHT:
							new_command.straight.pull_speed = std::stod(get(1));
							new_command.straight.ground_diameter = std::stod(get(2));
							new_command.straight.start_length = std::stod(get(3));
							new_command.straight.end_length = std::stod(get(4));
							new_command.straight.flat_number = get(8) == "1";
							new_command.straight.absolute = get(9) == "1";
							break;
						case command::TIMER:
							new_command.timer.time = std::stod(get(1));
							break;
						case command::TAPER:
							new_command.taper.pull_speed = std::stod(get(1));
							new_command.taper.start_diameter = std::stod(get(2));
							new_command.taper.start_length = std::stod(get(3));
							new_command.taper.end_diameter = std::stod(get(4));
							new_command.taper.end_length = std::stod(get(5));
							new_command.taper.start_and_end_length_number = std::stod(get(8));
							new_command.taper.absolute = get(9) == "1";
							break;
						}

						ret.cmds.push_back(new_command);

					}
					catch (std::invalid_argument e)
					{
						throw missing_data("NONE", cell);
					}
				}
			}
		}
	}
	return ret;
}
