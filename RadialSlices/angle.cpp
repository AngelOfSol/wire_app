#include "angle.h"

double normalize_angle(double angle)
{
	while (angle >= two_pi)
		angle -= two_pi;
	while (angle < 0)
		angle += two_pi;
	return angle;
}