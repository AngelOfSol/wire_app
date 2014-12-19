#pragma once
#include <vector>
#include <set>
#include <memory>
#include "vec2d.h"
#include "clipper.hpp"
class polygon
{
public:
	polygon(void);
	~polygon(void);

	std::vector<polygon> subtract(const polygon&) const;

	polygon sweep(const std::vector<vec2d>&) const;

	std::vector<vec2d> vertexes;

	ClipperLib::Path path(base_int) const;

	base_int highestDenominator() const;

	static polygon createUnion(const std::vector<polygon>&);

private:
	static std::vector<polygon> from(const ClipperLib::Paths&, base_int maxDenom);

};
