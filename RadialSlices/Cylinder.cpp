#include "Cylinder.h"
#include <iostream>
#include "make_poly.h"
Cylinder::Cylinder(void):turtle_(0, 0)
{
	Cylinder::State temp;
	temp.turtle_ = this->turtle_;
	temp.shape_ = this->shape_;
	this->states_.push_back(temp);
	
	this->scaleFactor_ = vec2d(1, 1);
}

Cylinder::~Cylinder(void)
{
}
void Cylinder::buildInitial(void)
{
	this->shape_.vertexes.clear();

	
	this->shape_.vertexes.push_back(vec2d(0, this->baseLine_));
	this->shape_.vertexes.push_back(vec2d(0, this->initialSize_.y));
	this->shape_.vertexes.push_back(this->initialSize_);
	this->shape_.vertexes.push_back(vec2d(this->initialSize_.x, this->baseLine_));

	this->turtle_ = this->initialTurtle_;

	this->states_.clear();
	this->moves_.clear();

	Cylinder::State temp;
	temp.turtle_ = this->turtle_;
	temp.shape_ = this->shape_;
	this->states_.push_back(temp);
};
void Cylinder::moveTool(vec2d absolutePosition)
{
	std::vector<vec2d> path;

	path.push_back(this->turtle_);

	this->turtle_ = absolutePosition;

	path.push_back(this->turtle_);

	polygon cuttingPolygon = this->turtleShape_.sweep(path);
	
	// we have our polygon to cut with!
	// so now just cut it up

	auto shapes = this->shape_.subtract(cuttingPolygon);

	std::sort(shapes.begin(), shapes.end(), [] (const polygon& lhs, const polygon& rhs)
	{
		return lhs.vertexes.size() > rhs.vertexes.size();
	});

	if(shapes.size() == 0)
	{
	} else
	{
		this->shape_ = shapes[0];
	}

	path.push_back(this->turtle_);
}
void Cylinder::pushMove(const Move& move, int_frac progress)
{
	if(move.id != MoveType::INVALID)
	{
		if(this->moves_.size() > 0 
			&& this->moves_.back().id == move.id 
			&& move.start == this->moves_.back().start 
			&& move.end == this->moves_.back().end)
			this->popMove();
		this->cut(move, progress);
		// push back the move for later
		this->moves_.push_back(move);
		Cylinder::State temp;
		temp.turtle_ = this->turtle_;
		temp.shape_ = this->shape_;
		this->states_.push_back(temp);
	}
}
void Cylinder::popMove()
{
	this->moves_.pop_back();
	this->states_.pop_back();
	this->shape_ = this->states_.back().shape_;
	this->turtle_ = this->states_.back().turtle_;
}

int_frac Cylinder::radiusAt(int_frac position) const
{
	if(position < 0)
		return 0;

	auto isLast = [] (vec2d vec)
	{
		return vec.y == 0 && vec.x != 0;
	};
	auto vertexes = this->shape_.vertexes;
	// shift all the ys downward
	for (auto& vert : vertexes)
	{
		vert.y -= this->baseLine_;
	}
	auto radZeroVert = std::find_if(vertexes.begin(), vertexes.end(), isLast);
	while(radZeroVert != vertexes.end())
	{
		vertexes.erase(radZeroVert);
		radZeroVert = std::find_if(vertexes.begin(), vertexes.end(), isLast);
	}
	// shift all the ys back upward
	for (auto& vert : vertexes)
	{
		vert.y += this->baseLine_;
	}
	std::stable_sort(vertexes.begin(), vertexes.end(), [] (const vec2d& left, const vec2d& right)
	{
		if(left.x == right.x)
		{
			return left.y < right.y;
		}
		return left.x < right.x;
	});

	int foundValue = -1;
	bool done = false;
	for(unsigned int i = 0; i < vertexes.size() && !done; i++)
	{
		if(foundValue != -1 && vertexes[i].x == position)
		{
			foundValue = i;
		} else if(foundValue != -1)
		{
			done = true;
		}
		else if(vertexes[i].x >= position)
		{
			// we've found our vertex
			// set set state to first vertex found
			foundValue = i;
		}
	}

	if(foundValue == -1)
		return 0;

	if(foundValue == 0)
		return vertexes[foundValue].y;

	auto right = vertexes[foundValue];
	auto left = vertexes[foundValue - 1];
	if(right.x == left.x)
	{
		return ((right.y) - this->reference_.y) / this->scaleFactor_.y + this->reference_.y;
	}
	

	
	int_frac M = (right.y - left.y) / (right.x - left.x);
	int_frac B = right.y - M * right.x;

	return  ((position * M + B) - this->reference_.y ) / this->scaleFactor_.y + this->reference_.y;
}

Move Cylinder::fixMove(const Move& moveArg) const
{
	Move move = moveArg;
	//if(move.id != MoveType::CIRCLE)
	{
		if(move.absolute)
		{
			move.end.x = move.end.x + this->reference_.x - this->turtle_.x;
		} else
		{

		}
		// radius is always absolute
		if(true)
		{
			move.end.y *= this->scaleFactor_.y;
			move.end.y = move.end.y + this->reference_.y - this->turtle_.y;
		} else
		{

		}
	
		if(!move.radiusAffected)
		{
			move.end.y = 0;
		}
		if(!move.positionAffected)
		{
			move.end.x = 0;
		}

		move.id = MoveType::NONE;
		return move;
	}
	return move;
}

void Cylinder::cut(const Move& moveArg, int_frac progress)
{
	// get a modifiable copy of the argument
	Move move = moveArg;

	if(move.id != MoveType::CIRCLE)
	{
		move = this->fixMove(move);

		move.end *= progress;

		std::vector<vec2d> path;

		path.push_back(this->turtle_);

		this->turtle_ += move.end;

		path.push_back(this->turtle_);

		polygon cuttingPolygon = this->turtleShape_.sweep(path);
	
		// we have our polygon to cut with!
		// so now just cut it up

		auto shapes = this->shape_.subtract(cuttingPolygon);

		std::sort(shapes.begin(), shapes.end(), [] (const polygon& lhs, const polygon& rhs)
		{
			return lhs.vertexes.size() > rhs.vertexes.size();
		});

		if(shapes.size() == 0)
		{
		} else
		{
			this->shape_ = shapes[0];
		}
		
	}
	else
	{
		auto offset = this->reference_ - this->turtle_;

		auto startVec = move.start - move.center;
		auto endVec = move.end - move.center;

		auto radius = startVec.length() * int_frac(1);
		auto startAngle = atan(startVec);
		auto endAngle = atan(endVec);
		auto changeAngle = endAngle - startAngle;

		std::vector<vec2d> path = make_arc(move.center, radius, startAngle, startAngle + changeAngle * progress, 15);

		for (auto& disp : path)
		{
			//disp += this->reference_;
		}
		//path.resize(std::max(2, (int)(progress * int_frac(30))));

		for (unsigned int i = 1; i < path.size(); i++)
		{
			Move newMove;
			newMove.absolute = true;
			newMove.end = path[i];
			newMove.id = MoveType::MOVE;
			this->cut(newMove, 1);
		}
	}

}

void Cylinder::cleanup()
{
	if (this->states_.size() > 100)
	{
		this->states_[1] = this->states_.back();
		this->states_.resize(2);
	}
}