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
			std::vector<Node*> newNodesRow;
			for (int x = 0; x < mazWidth; x++)
			{
				//make a brick
				Tile *B = new Tile();
				B->typeID = TileEnum::TRAVERSABLE;
				B->x = x;
				B->y = y;
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

double MazMap::Heuristic(Node &a, Node &b)
{
	return abs(b.x - a.x) + abs(b.y - a.y);
}

void MazMap::aStarPathFind()
{
	Node current = Node();
	current.x = rover.x;
	current.y = rover.y;
	current.typeID = map[rover.y][rover.x]->typeID;
	//s.g = 0 (s is the start node)
	current.g = 0;
	//s.h = GoalDistEstimate(s)
	//map[rover.y][rover.x]->h = Heuristic(map[rover.y][rover.x], map[exit.y][exit.x]);
	current.h = abs(exit.x - rover.x) + abs(exit.y - rover.y);
	//s.f = s.g + s.h + envirment cost
	current.priority = current.g + current.h;//+ ((int)map[rover.y][rover.x]->typeID)
	//s.parent = null
	current.xParent = -1;
	current.yParent = -1;

	//push s on Open
	open_nodes_map.push_back(current);

	int counter = 0;

	//while Open is not empty
	while (!open_nodes_map.empty())
	{
		counter++;
		if (counter >= 5000)
			break;

		//pop node n from Open  (n has the lowest f)
		Node current = open_nodes_map[0];
		open_nodes_map.erase(open_nodes_map.begin());

		//if n is a goal node
		if (current.x == exit.x && current.y == exit.y)
		{
			Node temp;
			//construct path
			while (current.xParent != -1 && current.yParent != -1)
			{
				solutionPath.push_back(map[current.y][current.x]);
				current = closed_nodes_map[findTile(closed_nodes_map, current.xParent, current.yParent)];
			}
			return;
		}

		//for each successor n' of n
		for (int index = 0; index < dir; index++)
		{
			int iDX = (current.x + dx[index]) >= mazWidth ? mazWidth - 1 : (current.x + dx[index]);
			iDX = (iDX < 0) ? 0 : iDX;

			int iDY = (current.y + dy[index]) >= mazHeight ? mazHeight - 1 : (current.y + dy[index]);
			iDY = (iDY < 0) ? 0 : iDY;


			//if n' is in Open or Closed, and n'.g <= newg
			if ((find(open_nodes_map, iDX, iDY) || (find(closed_nodes_map, iDX, iDY))))
			{
				int ListIndex = findTile(open_nodes_map, iDX, iDY);
				if (ListIndex > -1)
				{
					//newg = n.g + cost(n,n')
					double newCost = current.g + (int)open_nodes_map[ListIndex].typeID;

					if (newCost < open_nodes_map[ListIndex].g)
					{
						//openList[tempIndex].g = currentG;
						//openList[tempIndex].f = openList[tempIndex].h + openList[tempIndex].g;
						//openList[tempIndex].parentX = tempTile.xCoord;
						//openList[tempIndex].parentY = tempTile.yCoord;
						open_nodes_map[ListIndex].g = newCost + dCost[index];
						open_nodes_map[ListIndex].priority = open_nodes_map[ListIndex].h + open_nodes_map[ListIndex].g;
						open_nodes_map[ListIndex].xParent = current.x;
						open_nodes_map[ListIndex].yParent = current.y;

					}
					continue;
				}


				ListIndex = findTile(closed_nodes_map, iDX, iDY);
				if (ListIndex > -1)
				{
					//newg = n.g + cost(n,n')
					double newCost = current.g + (int)closed_nodes_map[ListIndex].typeID;
					if (newCost < closed_nodes_map[ListIndex].g)
					{
						//openList[tempIndex].g = currentG;
						//openList[tempIndex].f = openList[tempIndex].h + openList[tempIndex].g;
						//openList[tempIndex].parentX = tempTile.xCoord;
						//openList[tempIndex].parentY = tempTile.yCoord;
						closed_nodes_map[ListIndex].g = newCost + dCost[index];
						closed_nodes_map[ListIndex].priority = closed_nodes_map[ListIndex].h + closed_nodes_map[ListIndex].g;
						closed_nodes_map[ListIndex].xParent = current.x;
						closed_nodes_map[ListIndex].yParent = current.y;

						closed_nodes_map.push_back(closed_nodes_map[ListIndex]);
						closed_nodes_map.erase(closed_nodes_map.begin() + ListIndex);
					}
					continue;
				}
			}
			else
			{
				Node neighborsNode;
				neighborsNode.x = iDX;
				neighborsNode.y = iDY;
				neighborsNode.typeID = map[iDY][iDX]->typeID;
				neighborsNode.xParent = current.x;
				neighborsNode.yParent = current.y;
				//newg = n.g + cost(n,n')
				double newCost = current.g + (int)neighborsNode.typeID;
				//n'.parent = n
				//neighborsNode.parent = &current;
				//n'.g = newg
				neighborsNode.g = newCost + dCost[index];
				//n'.h = GoalDistEstimate( n' )//Heuristic(current, map[iDY][iDX]);
				neighborsNode.h = abs(exit.x - neighborsNode.x) + abs(exit.y - neighborsNode.y);

				//n'.f = n'.g + n'.h
				neighborsNode.priority = neighborsNode.g + neighborsNode.h;

				open_nodes_map.push_back(neighborsNode);
				if (find(closed_nodes_map, neighborsNode))
					closed_nodes_map.erase(closed_nodes_map.begin() + findTile(closed_nodes_map, neighborsNode));
			}
		}
		//push n onto Closed
		selectionSort(open_nodes_map, open_nodes_map.size());
		if (!find(closed_nodes_map, current))
			closed_nodes_map.push_back(current);
	}
	return;
}

void MazMap::swap(Node *xp, Node *yp)
{
	Node temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void MazMap::selectionSort(std::vector<Node> &arr, int n)
{
	int i, j, min_idx;

	// One by one move boundary of unsorted subarray 
	for (i = 0; i < n - 1; i++)
	{
		// Find the minimum element in unsorted array 
		min_idx = i;
		for (j = i + 1; j < n; j++)
			if (arr[j].priority < arr[min_idx].priority)
				min_idx = j;

		// Swap the found minimum element with the first element 
		Node temp = arr[min_idx];
		arr[min_idx] = arr[i];
		arr[i] = temp;
		//swap(arr[min_idx], arr[i]);
	}
}