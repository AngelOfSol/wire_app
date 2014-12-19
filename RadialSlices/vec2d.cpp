#include "vec2d.h"


vec2d::vec2d(int_frac x, int_frac y):x(x), y(y)
{
}

vec2d::vec2d(glm::vec2 data):x(data.x), y(data.y)
{
}


vec2d::~vec2d(void)
{
}

vec2d vec2d::operator -(void) const
{
	return vec2d(-this->x, -this->y);
}

vec2d vec2d::operator +(vec2d addend) const
{
	return vec2d(this->x + addend.x, this->y + addend.y);
}

vec2d vec2d::operator -(vec2d subtrahend) const
{
	return vec2d(this->x - subtrahend.x, this->y - subtrahend.y);
}

vec2d vec2d::operator *(int_frac multiplier) const
{
	return vec2d(this->x * multiplier, this->y * multiplier);
}

vec2d vec2d::operator /(int_frac multiplier) const
{
	return vec2d(this->x / multiplier, this->y / multiplier);
}

int_frac vec2d::dot(vec2d multiplier) const
{
	return this->x * multiplier.x + this->y * multiplier.y;
}
int_frac vec2d::cross(vec2d multiplier) const
{
	return this->x * multiplier.y - this->y * multiplier.x;
}

int_frac vec2d::length() const
{
	return sqrt(this->dot(*this));
}

vec2d vec2d::normalize() const
{
	return (*this) / this->length();
}
vec2d::operator glm::vec2(void)
{
	return glm::vec2(this->x, this->y);
}

bool vec2d::operator ==(vec2d compare) const
{
	return compare.x == this->x && compare.y == this->y;
}

vec2d operator *(int_frac arg0, vec2d arg1)
{
	return arg1 * arg0;
}
vec2d operator /(int_frac arg0, vec2d arg1)
{
	return arg1 / arg0;
}

vec2d minimize(vec2d a, vec2d b)
{
	return vec2d(std::min(a.x, b.x), std::min(a.y, b.y));
}
vec2d maximize(vec2d a, vec2d b)
{
	return vec2d(std::max(a.x, b.x), std::max(a.y, b.y));
}
int_frac atan(vec2d value)
{
	return int_frac(atan2((long double)value.y, (long double)value.x));
	
}
std::ostream& operator <<(std::ostream& out, vec2d val)
{
	out << (long double)val.x << ", " << (long double)val.y;
	return out;
}