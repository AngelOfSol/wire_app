#include "make_poly.h"

polygon make_circle(int_frac radius, vec2d offset, int resolution)
{
	polygon ret;

	int_frac portion(1, resolution);


	for (int i = 0; i < resolution; i++)
	{
		double angle = int_frac(2 * i) * PI_frac * portion;
		vec2d vector;
		vector.x = cos(angle);
		vector.y = sin(angle);
		vector *= radius;
		ret.vertexes.push_back(vector + offset);
	}

	return ret;
}
polygon make_iso_triangle(vec2d size)
{
	polygon ret;

	ret.vertexes.push_back(vec2d());
	ret.vertexes.push_back(vec2d(size.x / int_frac(2), size.y));
	ret.vertexes.push_back(vec2d(-size.x / int_frac(2), size.y));

	return ret;
}
polygon make_circle_tip(int_frac radius, int_frac height, int resolution)
{
	polygon ret;

	ret.vertexes.push_back(vec2d(-radius, height));
	ret.vertexes.push_back(vec2d(radius, height));

	for (int i = 0; i < resolution; i++)
	{
		auto angle = PI_frac * int_frac(i, resolution - 1);
		vec2d vector;
		vector.x = cos((double)angle);
		vector.y = -sin((double)angle);
		vector *= radius;
		ret.vertexes.push_back(vector + vec2d(0, radius));
	}


	return ret;
}
polygon make_rectangle(vec2d size, vec2d offset)
{
	polygon ret;

	ret.vertexes.push_back(offset);
	ret.vertexes.push_back(offset + vec2d(0, size.y));
	ret.vertexes.push_back(offset + vec2d(size.x, size.y));
	ret.vertexes.push_back(offset + vec2d(size.x, 0));

	return ret;
}
std::vector<vec2d> make_arc(vec2d center, int_frac radius, int_frac startAngle, int_frac endAngle, int resolution)
{
	std::vector<vec2d> ret;


	int_frac portion(1, resolution);

	auto diff = endAngle - startAngle;

	for (int i = 0; i <= resolution; i++)
	{
		auto angle = diff * portion * int_frac(i) + startAngle;
		vec2d vector;
		vector.x = cos((double)angle);
		vector.y = sin((double)angle);
		vector *= radius;
		ret.push_back(vector + center);
	}

	return ret;
	//resolution -= 2;
}