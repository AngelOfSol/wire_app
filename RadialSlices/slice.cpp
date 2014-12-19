#include "slice.h"
#include "sweep.h"

slice::slice(glm::vec2 initial_dimensions)
	: initial_dimensions(initial_dimensions)
{
	this->m_radii.push_back({ 0, 0 });
	this->m_radii.push_back({ 0, initial_dimensions.y });
	this->m_radii.push_back(initial_dimensions);
	this->m_radii.push_back({ initial_dimensions.x, 0 });
}
slice::slice(glm::vec2 initial_dimensions, std::vector<glm::vec2> radii)
	: initial_dimensions(initial_dimensions)
	, m_radii(std::move(radii))
{

}
slice::slice(glm::vec2 initial_dimensions, ClipperLib::Path p, glm::ivec2 scalar_by, glm::vec2 shifted_by)
	: initial_dimensions(initial_dimensions)
{
	for (auto ivec : p)
	{
		auto v = glm::vec2{ ivec.X, ivec.Y };
		v /= scalar_by;
		v -= shifted_by;
		this->m_radii.push_back(v);
	}
	auto zero_point = std::find(this->m_radii.begin(), this->m_radii.end(), glm::vec2{ 0, 0 });
	if (zero_point != this->m_radii.end())
	{
		std::rotate(this->m_radii.begin(), this->m_radii.end(), zero_point);

		if (this->m_radii.back().y != 0 && this->m_radii[1].y == 0)
		{
			std::rotate(this->m_radii.begin(), this->m_radii.begin() + 1, this->m_radii.end());
			std::reverse(this->m_radii.begin(), this->m_radii.end());
		}
	}
}


slice::~slice()
{
}
ClipperLib::Path slice::as_int_path(glm::ivec2 scalar, glm::vec2 shift) const
{

	ClipperLib::Path ret;
	for (auto v : this->m_radii)
	{
		v *= scalar;
		v += shift * glm::vec2{ abs(scalar.x), abs(scalar.y)};
		ret.push_back({ static_cast<ClipperLib::cInt>(v.x), static_cast<ClipperLib::cInt>(v.y) });
	}
	return ret;

}

std::vector<glm::vec2> slice::as_float_path() const
{
	std::vector<glm::vec2> ret;
	for (const auto& v : this->m_radii)
	{
		ret.push_back(v);
	}
	return ret;
}
int slice::points_at(float x) const
{
	int num_points = 0;
	for (const auto& v : this->m_radii)
	{
		if (v.x == x)
			num_points++;
	}
	return num_points;
}
void slice::slice_at(float x)
{
	for (auto iter = this->m_radii.begin() + 1; iter < this->m_radii.end(); iter++)
	{
		if (x < iter->x)
		{
			auto prev = iter - 1;

			auto slope = (prev->y - iter->y) / (prev->x - iter->x);
			auto x_param = x - iter->x;
			auto y_final = slope * x_param + iter->y;

			this->m_radii.insert(iter, {x, y_final});

			return;
		} if (x == iter->x)
		{
			this->m_radii.insert(iter, { x, iter->y });

			return;
		}
	}
}
void slice::cut(glm::ivec2 scale_this, glm::vec2 shift_this, slice clip, glm::ivec2 scale_clip, glm::vec2 shift_clip)
{
	auto subject_path = this->as_int_path(scale_this, shift_this);
	auto clip_path = clip.as_int_path(scale_clip, shift_clip);

	ClipperLib::Clipper c;
	c.AddPath(subject_path, ClipperLib::PolyType::ptSubject, true);
	c.AddPath(clip_path, ClipperLib::PolyType::ptClip, true);

	ClipperLib::Paths answer;

	c.Execute(ClipperLib::ClipType::ctDifference, answer);

	this->m_radii.clear();

	if (answer.size() >= 1)
	{
		ClipperLib::Path max{};
		for (const auto& v : answer)
		{
			if (max.size() < v.size())
				max = v;
		}
		for (auto ivec : max)
		{
			auto v = glm::vec2{ ivec.X, ivec.Y };
			v /= scale_this;
			v -= shift_this;
			this->m_radii.push_back(v);
		}
	}
}
std::vector<slice> slice::cut_at(float x) const
{
	std::vector<glm::vec2> left, right;
	for (auto v : this->m_radii)
	{
		if (v.x <= x)
		{
			left.emplace_back(v);
		}
		else
		{
			right.emplace_back(v);
		}
	}
	if (left.size() > 0 && right.size() > 0)
	{
		left.push_back({ x, 0 });
		right.push_back({ x, 0 });
	}

	std::vector<slice> ret;

	ret.emplace_back(slice(this->initial_dimensions, left));
	ret.emplace_back(slice(this->initial_dimensions, right));

	return ret;
}
slice slice::sweep(glm::ivec2 scale, glm::vec2 start, glm::vec2 stop) const
{
	ClipperLib::Path sweep_path = { 
			{ static_cast<ClipperLib::cInt>(start.x * scale.x), static_cast<ClipperLib::cInt>(start.y * scale.y) }
			, { static_cast<ClipperLib::cInt>(stop.x * scale.x), static_cast<ClipperLib::cInt>(stop.y * scale.y) }
	};

	auto sweep_object = this->as_int_path(scale);

	auto new_path = ::sweep(sweep_object, sweep_path);

	return slice(this->initial_dimensions, new_path);
}
slice slice::transform(glm::ivec2 scalar, glm::vec2 shift) const
{
	std::vector<glm::vec2> ret;
	for (auto v : this->m_radii)
	{
		v *= scalar;
		v += shift * glm::vec2{ abs(scalar.x), abs(scalar.y) };
		ret.push_back(v);
	}
	return slice(this->initial_dimensions * glm::vec2(scalar), ret);
}
slice slice::union_all(std::vector<slice> slices, glm::ivec2 scalar)
{
	ClipperLib::Paths union_these;

	auto current_shape = slices[0].as_int_path(scalar);

	for (unsigned int i = 1; i < slices.size(); i++)
	{
		auto new_shape = slices[i].as_int_path(scalar);

		ClipperLib::Clipper unioneer;
		unioneer.AddPath(current_shape, ClipperLib::PolyType::ptSubject, true);
		unioneer.AddPath(new_shape, ClipperLib::PolyType::ptClip, true);
		unioneer.AddPaths(union_these, ClipperLib::PolyType::ptClip, true);
		unioneer.Execute(ClipperLib::ClipType::ctUnion, union_these, ClipperLib::PolyFillType::pftNonZero, ClipperLib::PolyFillType::pftNonZero);
		current_shape = union_these[0];
	}

	return slice(slices[0].initial_dimensions, current_shape);
}
/*

base_int maxDenom = 1;
for(auto poly : data)
{
maxDenom = lcm(maxDenom, poly.highestDenominator());
}
//std::cout << "-----" << std::endl;
//std::cout << maxDenom << std::endl;
polygon base = data[0];

for(unsigned int i = 1; i < data.size(); i++)
{
ClipperLib::Clipper clipper;
clipper.AddPath(base.path(maxDenom), ClipperLib::PolyType::ptSubject, true);
clipper.AddPath(data[i].path(maxDenom), ClipperLib::PolyType::ptClip, true);

ClipperLib::Paths returnedPaths;
clipper.Execute(ClipperLib::ClipType::ctUnion, returnedPaths, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

base = polygon::from(returnedPaths, maxDenom)[0];

}

return base;
*/
/*

ClipperLib::Path polygon::path(base_int denom) const
{
ClipperLib::Path ret;
for(auto v : this->vertexes)
{
ret.push_back(ClipperLib::IntPoint((ClipperLib::cInt)(v.x.numerator() * (denom / v.x.denominator())), (ClipperLib::cInt)(v.y.numerator() * (denom / v.y.denominator()))));
}
return ret;
}
*/