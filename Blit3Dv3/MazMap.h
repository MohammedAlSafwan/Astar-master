#pragma once
#include "Tile.h"
#include "Node.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class MazMap
{
private:

	struct Rover
	{
		int x = -1;
		int y = -1;
	};

	struct Exit
	{
		int x = -1;
		int y = -1;
	};

	struct PathTile
	{
		int x = -1;
		int y = -1;
		PathTile(int x, int y)
		{
			this->x = x;
			this->y = y;
		};

	};


public:

	int mazWidth = 0;							//Map width
	int mazHeight = 0;							//Map height
	int counter = 0;							//counter for how many successful tiles are there
	Rover rover;								//the rover
	Exit exit;									//the exit
	std::vector<std::vector<Tile *>> map;		//Map tiles
	std::vector<Tile*> solutionPath;			//The successful solution path

	MazMap();
	~MazMap();
	void LoadMap(std::string fileName);
	void SaveMap(std::string fileName);

	std::vector<Node> closed_nodes_map; // map of closed (tried-out) nodes
	std::vector<Node> open_nodes_map; // map of open (not-yet-tried) nodes
	//std::vector<std::vector<Tile>> dir_map; // map of directions
	static const int dir = 8; // number of possible directions to go at any position
	//int dx[dir] =		{ 1, 1, 0, -1, -1, -1, 0, 1 };
	int dx[dir] = { -1, 1, 0, 0, -1, 1, -1, 1 };
	//int dy[dir] =		{ 0, 1, 1, 1, 0, -1, -1, -1 };
	int dy[dir] = { 0, 0, -1, 1, 1, 1, -1, -1 };
	float dCost[dir] = { 0, 0, 0, 0, 0.001, 0.001, 0.001, 0.001 };
	//static const int dir = 4; // number of possible directions to go at any position
	//int dx[dir] = { -1, 1, 0, 0 };
	//int dy[dir] = { 0, 0, -1, 1 };

	void MazMap::aStarPathFind();
	double Heuristic(Node &a, Node &b);

	bool find(std::vector<Node> vec, Node tileToFine) {
		for (Node tile : vec) {
			if (tile.x == tileToFine.x && tile.y == tileToFine.y) return true;
		}
		return false;
	}
	bool find(std::vector<Node> vec, int x, int y) {
		for (Node tile : vec) {
			if (tile.x == x && tile.y == y) return true;
		}
		return false;
	}

	int findTile(std::vector<Node> &vec, Node tileToFine) {
		int index = 0;
		for (Node tile : vec) {
			if (tile.x == tileToFine.x && tile.y == tileToFine.y) return index;
			index++;
		}

		return -1;
	}
	int findTile(std::vector<Node> &vec, int x, int y) {
		int index = 0;
		for (Node tile : vec) {
			if (tile.x == x && tile.y == y) return index;
			index++;
		}

		return -1;
	}

	void selectionSort(std::vector<Node> &arr, int n);
	void swap(Node *xp, Node *yp);
};
