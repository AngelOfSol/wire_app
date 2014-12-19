#pragma once

#include "glm_includes.h"
#include "typedefs.h"
#include "polygon.h"
#include "segment_reader.h"
#include <vector>
#include "Vec2D.h"

class Cylinder
{
public:
	
	Cylinder(void);
	~Cylinder(void);

	void moveTool(vec2d);

	Move fixMove(const Move&) const;

	void pushMove(const Move&, int_frac progress);

	void popMove(void);

	int numMoves() const { return this->moves_.size(); };

	int_frac radiusAt(int_frac position) const;

	polygon& turtleShape() { return this->turtleShape_; };

	void buildInitial();

	void cleanup();

	std::vector<vec2d> vertexes() const { return this->shape_.vertexes;};

	vec2d& scale() { return this->scaleFactor_; };
	vec2d& reference() { return this->reference_; };
	
	vec2d& initialSize() { return this->initialSize_; };
	vec2d& initialTurtle() { return this->initialTurtle_; };
	
	vec2d scale() const { return this->scaleFactor_; };
	vec2d reference() const { return this->reference_; };

	vec2d initialSize() const { return this->initialSize_; };
	vec2d initialTurtle() const { return this->initialTurtle_; };

	vec2d turtle() const { return this->turtle_; };

	int_frac& baseLine() { return this->baseLine_; };
	int_frac baseLine() const { return this->baseLine_; };

	Move top() const { return this->moves_.back();};

private:
	vec2d scaleFactor_;
	vec2d reference_;
	struct State
	{
		vec2d turtle_;
		polygon shape_;
	};
	void cut(const Move&, int_frac progress);
	
	std::vector<Move> moves_;
	std::vector<State> states_;

	polygon shape_;
	polygon turtleShape_;

	vec2d turtle_;

	vec2d initialSize_;
	vec2d initialTurtle_;

	int_frac baseLine_;

};

