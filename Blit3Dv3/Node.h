#pragma once
#include "Tile.h"

class Node
{
public:
	// current position
	int x = 0;
	int y = 0;

	TileEnum typeID = TileEnum::UNKNOWN; //envirment cost
	float g = 0;			 //actual cost
	int h = 0;			 //Heuristic
	//priority=level+remaining distance estimate
	float priority = 0;	 //smaller: higher priority #f
	//Tile* parent = NULL;
	int xParent = 0;
	int yParent = 0;

	Node();
	~Node();

	// Estimation function for the remaining distance to the goal.
	const int & estimate(const int & xDest, const int & yDest) const
	{
		static int xd, yd, d;
		xd = xDest - x;
		yd = yDest - y;

		// Euclidian Distance
		d = static_cast<int>(sqrt(xd*xd + yd * yd));

		// Manhattan distance
		//d=abs(xd)+abs(yd);

		// Chebyshev distance
		//d=max(abs(xd), abs(yd));

		return(d);
	}

};

