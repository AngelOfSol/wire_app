#include "sweep.h"

#include <boost\polygon\polygon.hpp>

typedef boost::polygon::point_data<ClipperLib::cInt> point;
typedef boost::polygon::polygon_set_data<ClipperLib::cInt> polygon_set;
typedef boost::polygon::polygon_with_holes_data<ClipperLib::cInt> polygon;
typedef std::pair<point, point> edge;
using namespace boost::polygon::operators;

void convolve_two_segments(std::vector<point>& figure, const edge& a, const edge& b) {
	using namespace boost::polygon;
	figure.clear();
	figure.push_back(point(a.first));
	figure.push_back(point(a.first));
	figure.push_back(point(a.second));
	figure.push_back(point(a.second));
	convolve(figure[0], b.second);
	convolve(figure[1], b.first);
	convolve(figure[2], b.first);
	convolve(figure[3], b.second);
}

template <typename itrT1, typename itrT2>
void convolve_two_point_sequences(polygon_set& result, itrT1 ab, itrT1 ae, itrT2 bb, itrT2 be) {
	using namespace boost::polygon;
	if (ab == ae || bb == be)
		return;
	point first_a = *ab;
	point prev_a = *ab;
	std::vector<point> vec;
	polygon poly;
	++ab;
	for (; ab != ae; ++ab) {
		point first_b = *bb;
		point prev_b = *bb;
		itrT2 tmpb = bb;
		++tmpb;
		for (; tmpb != be; ++tmpb) {
			convolve_two_segments(vec, std::make_pair(prev_b, *tmpb), std::make_pair(prev_a, *ab));
			set_points(poly, vec.begin(), vec.end());
			result.insert(poly);
			prev_b = *tmpb;
		}
		prev_a = *ab;
	}
}

template <typename itrT>
void convolve_point_sequence_with_polygons(polygon_set& result, itrT b, itrT e, const std::vector<polygon>& polygons) {
	using namespace boost::polygon;
	for (std::size_t i = 0; i < polygons.size(); ++i) {
		convolve_two_point_sequences(result, b, e, begin_points(polygons[i]), end_points(polygons[i]));
		for (polygon_with_holes_traits<polygon>::iterator_holes_type itrh = begin_holes(polygons[i]);
			itrh != end_holes(polygons[i]); ++itrh) {
			convolve_two_point_sequences(result, b, e, begin_points(*itrh), end_points(*itrh));
		}
	}
}

void convolve_two_polygon_sets(polygon_set& result, const polygon_set& a, const polygon_set& b) {
	using namespace boost::polygon;
	result.clear();
	std::vector<polygon> a_polygons;
	std::vector<polygon> b_polygons;
	a.get(a_polygons);
	b.get(b_polygons);
	for (std::size_t ai = 0; ai < a_polygons.size(); ++ai) {
		convolve_point_sequence_with_polygons(result, begin_points(a_polygons[ai]),
			end_points(a_polygons[ai]), b_polygons);
		for (polygon_with_holes_traits<polygon>::iterator_holes_type itrh = begin_holes(a_polygons[ai]);
			itrh != end_holes(a_polygons[ai]); ++itrh) {
			convolve_point_sequence_with_polygons(result, begin_points(*itrh),
				end_points(*itrh), b_polygons);
		}
		for (std::size_t bi = 0; bi < b_polygons.size(); ++bi) {
			polygon tmp_poly = a_polygons[ai];
			result.insert(convolve(tmp_poly, *(begin_points(b_polygons[bi]))));
			tmp_poly = b_polygons[bi];
			result.insert(convolve(tmp_poly, *(begin_points(a_polygons[ai]))));
		}
	}
}

ClipperLib::Path sweep(ClipperLib::Path shape, ClipperLib::Path sweep_path)
{
	std::vector<point> shape_points;

	for (const auto& v : shape)
	{
		shape_points.push_back({ v.X, v.Y });
	}

	polygon_set shifted_polys;
	for (const auto& shift_point : sweep_path)
	{
		std::vector<point> temp_points;
		for (const auto& v : shape)
		{
			temp_points.push_back({ v.X + shift_point.X, v.Y + shift_point.Y });
		}
		polygon temp_poly;
		boost::polygon::set_points(temp_poly, temp_points.begin(), temp_points.end());

		shifted_polys += temp_poly;
	}




	polygon shape_polygon;
	boost::polygon::set_points(shape_polygon, shape_points.begin(), shape_points.end());
	std::vector<polygon> shapes = { shape_polygon };

	std::vector<point> sweep_line;

	for (const auto& v : sweep_path)
	{
		sweep_line.push_back({ v.X, v.Y });
	}

	polygon_set result_set;

	convolve_point_sequence_with_polygons(result_set, sweep_line.begin(), sweep_line.end(), shapes);

	result_set += shifted_polys;

	std::vector<polygon> result_polys;

	result_set.get(result_polys);

	ClipperLib::Path to_simplify;
	ClipperLib::Paths result_paths;

	for (auto v : result_polys[0])
	{
		to_simplify.push_back({ v.x(), v.y() });
	}
	ClipperLib::SimplifyPolygon(to_simplify, result_paths, ClipperLib::pftNonZero);

	return result_paths[0];
}