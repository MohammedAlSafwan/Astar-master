#pragma once
#include "MazMap.h"
#include <assert.h>
MazMap::MazMap()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	MazMap::MazMap()
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   MazMap constructor
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
}

MazMap::~MazMap()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	MazMap::~MazMap()
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   MazMap deconstructor
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	for (auto row : map)
	{
		for (auto col : row)
		{
			col->~Tile();
		}
	}
	//clear solution path
	solutionPath.clear();

	//clear the tiles
	map.clear();
}

void MazMap::LoadMap(std::string fileName)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void MazMap::LoadMap(std::string fileName)
	//
	// Method parameters	:	fileName - the file name to be loaded.
	//
	// Method return		:	void
	//
	// Synopsis				:   Load a map from a given file
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	for (auto row : map)
	{
		for (auto col : row)
		{
			col->~Tile();
		}
	}
	map.clear();
	//open file
	std::ifstream myfile(fileName);

	if (myfile.is_open())
	{
		//read in # of bricks
		myfile >> mazWidth;
		myfile >> mazHeight;

		//init cols
		for (int y = 0; y < mazHeight; y++)
		{
			//init rows
			std::vector<Tile*> newRow;

			for (int x = 0; x < mazWidth; x++)
			{
				//make a brick
				Tile *B = new Tile();
				B->typeID = TileEnum::TRAVERSABLE;
				B->x = x;
				B->y = y;
				B->g = 0;
				B->h = 0;
				B->level = 0;
				B->priority = 0;
				newRow.push_back(B);
			}
			map.push_back(newRow);

			/*tileList[y].reserve(mazWidth);*/
		}

		int type = 0;
		int posX = -1;
		int posY = 0;
		//std::string line;
		while (!myfile.eof())
		{
			posX++;

			if (posX == mazWidth)
				posY++;

			posX = posX % mazWidth;

			if (posY == mazHeight)
				break;
			myfile >> type;
			map[posY][posX]->typeID = (TileEnum)type;

			switch ((TileEnum)type) {
			case TileEnum::START:
				rover.x = posX;
				rover.y = posY;
				break;
			case TileEnum::END:
				exit.x = posX;
				exit.y = posY;
				break;
			default:
				break;
			}
		}
		//while (!myfile.eof())
		//{
		//	//make a tile
		//	myfile >> t;

		//	//width
		//	myfile >> posX;

		//	//height
		//	myfile >> posY;


		//	map[posY][posX]->typeID = (TileEnum)t;

		//	//if rover
		//	if ((TileEnum)t == TileEnum::ROVER)
		//	{
		//		//make rover
		//		rover.x = posX;
		//		rover.y = posY;
		//	}

		//	//if exit
		//	if ((TileEnum)t == TileEnum::END)
		//	{
		//		//make exit
		//		exit.x = posX;
		//		exit.y = posY;
		//	}
		//}
		//close when done
		myfile.close();
	}
}

void MazMap::SaveMap(std::string fileName)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void MazMap::SaveMap(std::string fileName)
	//
	// Method parameters	:	fileName - the file name to be saved.
	//
	// Method return		:	void
	//
	// Synopsis				:   Save a map from a given file
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//open file
	std::ofstream myfile;
	myfile.open(fileName, std::ios::trunc);

	if (myfile.is_open())
	{
		//write out # of bricks
		myfile << map.size() << std::endl;
		myfile << map[0].size() << std::endl;
		//write out each brick
		for (int y = 0; y < map.size(); y++)
		{
			for (int x = 0; x < map[0].size(); x++)
			{
				Tile *saveTile = map[y][x];
				if (saveTile->typeID != TileEnum::TRAVERSABLE)
				{
					myfile << (int)saveTile->typeID << " " << x << " " << y << std::endl;
				}
			}
		}

		myfile.close();
	}
}

double MazMap::Heuristic(Tile &a, Tile &b)
{
	return abs(b.x - a.x) + abs(b.y - a.y);
}

void MazMap::aStarPathFind()
{

	//s.g = 0 (s is the start node)
	map[rover.y][rover.x]->g = 0;
	//s.h = GoalDistEstimate(s)
	//map[rover.y][rover.x]->h = Heuristic(map[rover.y][rover.x], map[exit.y][exit.x]);
	map[rover.y][rover.x]->h = abs(map[exit.y][exit.x]->x - map[rover.y][rover.x]->x) + abs(map[exit.y][exit.x]->y - map[rover.y][rover.x]->y);
	//s.f = s.g + s.h + envirment cost
	map[rover.y][rover.x]->priority = map[rover.y][rover.x]->g + map[rover.y][rover.x]->h;//+ ((int)map[rover.y][rover.x]->typeID)
	//s.parent = null
	map[rover.y][rover.x]->parent = NULL;

	//push s on Open
	open_nodes_map.push_back(map[rover.y][rover.x]);

	int counter = 0;

	//while Open is not empty
	while (!open_nodes_map.empty())
	{
		counter++;
		if (counter >= 5000)
			break;

		//pop node n from Open  (n has the lowest f)
		Tile* current = open_nodes_map[0];
		open_nodes_map.erase(open_nodes_map.begin());

		//assert((current->x != 39 || current->y != 2) && "potato");

		//if n is a goal node
		if (current->x == exit.x && current->y == exit.y)
		{
			//construct path
			while (current->parent != NULL)
			{
				solutionPath.push_back(current);
				current = current->parent;
			}
			return;
		}

		//for each successor n' of n
		for (int index = 0; index < dir; index++)
		{
			int iDX = (current->x + dx[index]) >= mazWidth ? mazWidth - 1 : (current->x + dx[index]);
			iDX = (iDX < 0) ? 0 : iDX;

			int iDY = (current->y + dy[index]) >= mazHeight ? mazHeight - 1 : (current->y + dy[index]);
			iDY = (iDY < 0) ? 0 : iDY;

			if (iDX == 39 && iDY == 2)
			{
				int x = 0;
			}
			//newg = n.g + cost(n,n')
			double newCost = current->g + (int)map[iDY][iDX]->typeID;
			//if n' is in Open or Closed, and n'.g <= newg
			if ((find(open_nodes_map, map[iDY][iDX]) || (find(closed_nodes_map, map[iDY][iDX]))))
			{
				int ListIndex = findTile(open_nodes_map, map[iDY][iDX]);
				if (ListIndex > -1)
				{
					if (newCost < open_nodes_map[ListIndex]->g)
					{
						//openList[tempIndex].g = currentG;
						//openList[tempIndex].f = openList[tempIndex].h + openList[tempIndex].g;
						//openList[tempIndex].parentX = tempTile.xCoord;
						//openList[tempIndex].parentY = tempTile.yCoord;
						open_nodes_map[ListIndex]->g = newCost;
						open_nodes_map[ListIndex]->priority = open_nodes_map[ListIndex]->h + open_nodes_map[ListIndex]->g;
						open_nodes_map[ListIndex]->parent = map[current->y][current->x];

					}
					continue;
				}


				ListIndex = findTile(closed_nodes_map, map[iDY][iDX]);
				if (ListIndex > -1)
				{
					if (newCost < closed_nodes_map[ListIndex]->g)
					{
						//openList[tempIndex].g = currentG;
						//openList[tempIndex].f = openList[tempIndex].h + openList[tempIndex].g;
						//openList[tempIndex].parentX = tempTile.xCoord;
						//openList[tempIndex].parentY = tempTile.yCoord;
						open_nodes_map[ListIndex]->g = newCost;
						open_nodes_map[ListIndex]->priority = open_nodes_map[ListIndex]->h + open_nodes_map[ListIndex]->g;
						open_nodes_map[ListIndex]->parent = map[current->y][current->x];

						open_nodes_map.push_back(closed_nodes_map[ListIndex]);
						closed_nodes_map.erase(closed_nodes_map.begin() + ListIndex);
					}
					continue;
				}
			}
			else
			{

				//n'.parent = n
				map[iDY][iDX]->parent = map[current->y][current->x];
				//n'.g = newg
				map[iDY][iDX]->g = newCost;
				//n'.h = GoalDistEstimate( n' )//Heuristic(current, map[iDY][iDX]);
				map[iDY][iDX]->h = abs(map[iDY][iDX]->x - current->x) + abs(map[iDY][iDX]->y - current->y);

				//n'.f = n'.g + n'.h
				map[iDY][iDX]->priority = map[iDY][iDX]->g + map[iDY][iDX]->h;

				open_nodes_map.push_back(map[iDY][iDX]);
				if (find(closed_nodes_map, map[iDY][iDX]))
					closed_nodes_map.erase(closed_nodes_map.begin() + findTile(closed_nodes_map, map[iDY][iDX]));
				////if n' is in Closed
				//if (find(closed_nodes_map, map[iDY][iDX]))
				//{
				//	//remove it from Closed
				//	//closed_nodes_map.erase(closed_nodes_map + )
				//	closed_nodes_map.erase(closed_nodes_map.begin() + findTile(closed_nodes_map, map[iDY][iDX]));
				//}
				////if n' is not yet in Open
				//if (!find(open_nodes_map, map[iDY][iDX]))
				//{
				//	//push n' on Open
				//	open_nodes_map.push_back(map[iDY][iDX]);
				//	selectionSort(open_nodes_map, open_nodes_map.size());
				//	//std::sort(open_nodes_map.begin(), open_nodes_map.end(),greater);
				//}

			}
		}
		//push n onto Closed
		selectionSort(open_nodes_map, open_nodes_map.size());
		if (!find(closed_nodes_map, map[current->y][current->x]))
			closed_nodes_map.push_back(map[current->y][current->x]);
	}
	return;
}

void MazMap::swap(Tile &xp, Tile &yp)
{
	Tile temp = xp;
	xp = yp;
	yp = temp;
}

void MazMap::selectionSort(std::vector<Tile*> &arr, int n)
{
	int i, j, min_idx;

	// One by one move boundary of unsorted subarray 
	for (i = 0; i < n - 1; i++)
	{
		// Find the minimum element in unsorted array 
		min_idx = i;
		for (j = i + 1; j < n; j++)
			if (arr[j]->priority < arr[min_idx]->priority)
				min_idx = j;

		// Swap the found minimum element with the first element 
		//Tile* temp = open_nodes_map[min_idx];
		//open_nodes_map[min_idx] = open_nodes_map[i];
		//open_nodes_map[i] = temp;
		swap(*arr[min_idx], *arr[i]);
	}
}