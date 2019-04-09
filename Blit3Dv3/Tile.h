#pragma once
#include <queue>
#include<math.h>
#define TILE_WIDTH 20
#define TILE_HEIGHT 20

//traversable
enum class TileEnum {
	UNTRAVERSABLE = 0,
	TRAVERSABLE, //1
	START,		 //2
	END,		 //3
	PASSED,		 //4
	ROVER,		 //5
	UNKNOWN,	 //6
	SAND,		 //7
	MUD,		 //8
	ROCKY,		 //9
	WATER,		 //10
	PATH,		 //11
	SIZE
};

class Tile
{

public:
	// current position
	int x = 0;
	int y = 0;
	// total distance already travelled to reach the node
	int level = 0;


	TileEnum typeID = TileEnum::UNKNOWN; //envirment cost
	int g = 0;			 //actual cost
	int h = 0;			 //Heuristic
	//priority=level+remaining distance estimate
	int priority = 0;	 //smaller: higher priority #f
	/*Tile* parent = NULL;*/
	int xParent = -1;
	int yParent = -1;

	Tile();
	~Tile();

	Tile(int xPosition, int yPosition, int levelIn, int priorityIn)
	{
		x = xPosition; y = yPosition; level = levelIn; priority = priorityIn;
	}

	int getxPos() const { return x; }
	int getyPos() const { return y; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int & xDest, const int & yDest)
	{
		priority = level + estimate(xDest, yDest) * 10; //A*
	}

	// give better priority to going strait instead of diagonally
	void nextLevel(const int & i) // i: direction
	{
		//level += (dir == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
		level += i % 2 == 0 ? 10 : 14;
	}

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

	// Determine priority (in the priority queue)
	bool Tile::operator<(const Tile& val) const
	{
		return (priority > val.priority);
	}
	friend bool Tile::operator<(const Tile & a, const Tile & b) {
		return a.priority > b.priority;
	}
	//friend bool operator==(const Tile & a, const Tile & b) {
	//	return a.getPriority() == b.getPriority();
	//}

};

