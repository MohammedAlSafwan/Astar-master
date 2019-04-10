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
	int x = -1;
	int y = -1;
	TileEnum typeID = TileEnum::UNKNOWN; //envirment cost


	Tile();
	~Tile();



};

