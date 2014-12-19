#pragma once
#include <math.h>
#include <algorithm>
#include "typedefs.h"
#include "glm_includes.h"
#include <iostream>

class vec2d
{
public:
	vec2d(int_frac x = 0, int_frac y = 0);
	vec2d(glm::vec2);
	~vec2d(void);
	
	vec2d operator -(void) const;
	
	vec2d operator +(vec2d) const;
	vec2d operator -(vec2d) const;
	vec2d operator *(int_frac) const;
	vec2d operator /(int_frac) const;

	int_frac dot(vec2d) const;
	int_frac cross(vec2d) const;
	vec2d normalize() const;
	int_frac length() const;
	
	vec2d& operator +=(vec2d arg0) { *this = *this + arg0; return *this; };
	vec2d& operator -=(vec2d arg0) { *this = *this - arg0; return *this; };
	vec2d& operator *=(int_frac arg0) { *this = *this * arg0; return *this; };
	vec2d& operator /=(int_frac arg0) { *this = *this / arg0; return *this; };

	bool operator ==(vec2d) const;
	bool operator !=(vec2d arg0) const { return !((*this) == arg0); };


	int_frac x;
	int_frac y;

	operator glm::vec2();
};


vec2d minimize(vec2d, vec2d);
vec2d maximize(vec2d, vec2d);

std::ostream& operator <<(std::ostream&, vec2d);

int_frac atan(vec2d);