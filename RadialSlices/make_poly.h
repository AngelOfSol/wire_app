#pragma once

#include "polygon.h"
#include <vector>
polygon make_iso_triangle(vec2d size );
polygon make_rectangle(vec2d size, vec2d offest);
polygon make_circle(int_frac radius, vec2d offset, int resolution);
polygon make_circle_tip(int_frac radius, int_frac height, int resolution);

std::vector<vec2d> make_arc(vec2d center, int_frac radius, int_frac startAngle, int_frac endAngle, int resolution);

static const long double PI = atan(1.0) * 4.0;
static const int_frac PI_frac = int_frac(PI);