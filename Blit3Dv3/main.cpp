/*
	Simple example of an Arkinoid level editor
*/

#include "Blit3D.h"
#include "Camera.h"
#include <chrono>
Blit3D *blit3D = NULL;

//memory leak detection
#define CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#include "MazMap.h"

#define MAX_DRAW_X	100
#define MAX_DRAW_Y	100

#define TILE_WIDTH 20
#define TILE_HEIGHT 20

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define PAN_SCREEN_BY 1.0f
#define PAN_TILES_BY 1

Camera2D *camera; //pans the view
MazMap *map; //bricks for a level

void initSprits();
void initFont();
void initMaze();
void initCamera();
void solveMap();
void drawFont();
void drawMap();

//GLOBAL DATA
std::vector<Sprite *> spriteList;
TileEnum editBrick = TileEnum::UNTRAVERSABLE; //brick we are currently editing with

int offX = 0;
int offY = 0;

float timeStep = 5.f / 60.f; //one 60th of a second
float elapsedTime = 0; //used for calculating time passed

int showTile = 0;

AngelcodeFont *font = NULL;//caviar57font
std::string infoTxt = "";

glm::vec2 cursor;

void Init()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void Init() 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   initilize the sprits, the font, the map, and camera.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	initSprits();
	initFont();
	initMaze();
	initCamera();
}

void initSprits()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	initSprits() 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   initilize the sprits.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 3; x++) {
			spriteList.push_back(blit3D->MakeSprite(x*TILE_WIDTH, y*TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, "Media\\BRICKS.png"));
		}
	}

	spriteList.push_back(blit3D->MakeSprite(20, 0, TILE_WIDTH, TILE_HEIGHT, "Media\\BRICKS.png"));
	spriteList.push_back(blit3D->MakeSprite(40, 0, TILE_WIDTH, TILE_HEIGHT, "Media\\BRICKS.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 20, TILE_WIDTH, TILE_HEIGHT, "Media\\BRICKS.png"));
	spriteList.push_back(blit3D->MakeSprite(20, 20, TILE_WIDTH, TILE_HEIGHT, "Media\\BRICKS.png"));
	spriteList.push_back(blit3D->MakeSprite(40, 20, TILE_WIDTH, TILE_HEIGHT, "Media\\BRICKS.png"));
}

void initFont()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void initFont() 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   initilize the font.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//load an Angelcode binary32 font file
	font = blit3D->MakeAngelcodeFontFromBinary32("Media\\CaviarDreams57.bin");
}

void initMaze()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void initMaze() 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   initilize the map.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	map = new MazMap();
	map->LoadMap("map.txt");
	//map->LoadMap("Alex_test_map.txt");
	//map->LoadMap("Alex_test_map1024.txt");
	//map->LoadMap("Dav_test_map.txt");
	//map->LoadMap("Ryan_test_map.txt");
}

void initCamera()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void initCamera() 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   initilize the camera.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//make a camera
	camera = new Camera2D();

	//set it's valid pan area
	//camera->minX = blit3D->screenWidth / 2;
	//camera->minY = blit3D->screenHeight / 2;
	//camera->maxX = blit3D->screenWidth * 2 - blit3D->screenWidth / 2;
	//camera->maxY = blit3D->screenHeight / 2 + 400;
	camera->minX = blit3D->screenWidth / 2;
	camera->minY = blit3D->screenHeight / 2;
	camera->maxX = map->mazWidth * TILE_WIDTH;
	camera->maxY = map->mazHeight * TILE_HEIGHT;

	camera->PanTo(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
	//camera->PanTo(map->rover.x * TILE_WIDTH, map->rover.y * TILE_WIDTH);

}

void solveMap()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void solveMap() 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Try to solve the puzzle and print output to the console and the screen.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	printf("Start Solving!\n");
	infoTxt = "Start Solving...\n";

	//Start counting the time it takes to solve the puzzle
	auto start = std::chrono::high_resolution_clock::now();

	//return a text discriping if the path was found or not
	//infoTxt = map->findEnd() ?
	//	"Found the end \nx =" + std::to_string(map->exit.x) + "\ny = " + std::to_string(map->exit.y) + "\nSteps = " + std::to_string(map->counter) + "\n" :
	//	"Couldn't find the end.";
	//infoTxt += map->pathFind(map->rover.x,map->rover.y,map->exit.x,map->exit.y);
	map->aStarPathFind();
	int x = 0;
	x++;
	//Finish counting
	auto finish = std::chrono::high_resolution_clock::now();
	//Print the time needed to find the game
	printf("Path: %s\n", infoTxt.c_str());
	printf("Time to finish ");
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " ns\n";
	printf("%d steps\n", map->solutionPath.size());
	printf("%f steps a day", (map->counter / 10000.f));
}

void DeInit(void)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void DeInit(void) 
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Try to solve the puzzle and print output to the console and the screen.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//Delete camera
	if (camera != NULL) delete camera;

	//Delete map
	if (map != NULL) map->~MazMap();

	// DO NOT delete the font!
	//If we need to delete a font at runtime, call DeleteFont().

	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void Update(double seconds)
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Update the camera and the map. The rover if there is a solution.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	elapsedTime += (float)seconds;
	//if there is a solution
	if (map->solutionPath.size() > 0)
	{
		//if passed or equal to the time step
		if (elapsedTime >= timeStep)
		{
			//make sure that the elapsed time is less than time step
			while (elapsedTime >= timeStep)
			{
				elapsedTime -= timeStep;
			}

			//advance rover
			/*map->rover.x = map->solutionPath[showTile].x;
			map->rover.y = map->solutionPath[showTile].y;*/

			//make sure that the rover is in between bonderies
			if (showTile >= map->solutionPath.size())
			{
				showTile = map->solutionPath.size() - 1;
			}
			else
			{
				showTile++;
			}
		}
	}

	//Update the camera
	camera->Update((float)seconds);
}

void Draw(void)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void Draw(void)
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Draw the camera, map, fond, and square under cursor.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//clear screen before drawing
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	//clear colour: r,g,b,a 	
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//MUST draw camera first!
	camera->Draw();

	//make sure the rover is set before panning the camera
	if (map->rover.x != -1 || map->rover.y != -1)
	{
		//offX = map->rover.x;
		//offY = map->rover.y;
		//Pan the camera to the rover location
		camera->PanTo(map->rover.x * TILE_WIDTH, map->rover.y * TILE_WIDTH);
	}

	//Draw the map.
	drawMap();
	//if (map->open_nodes_map.size() > 0)
	//	for (int index = 0; index < map->open_nodes_map.size(); index++)
	//	{
	//		spriteList[9]->Blit(
	//			map->open_nodes_map[index]->x * TILE_WIDTH,
	//			map->open_nodes_map[index]->y * TILE_HEIGHT);
	//	}

	if (map->solutionPath.size() > 0)
		for (int index = 0; index < map->solutionPath.size(); index++)
		{
			spriteList[10]->Blit(
				map->solutionPath[index]->x * TILE_WIDTH,
				map->solutionPath[index]->y * TILE_HEIGHT);
		}

	//if (map->closed_nodes_map.size() > 0)
	//	for (int index = 0; index < map->closed_nodes_map.size(); index++)
	//	{
	//		spriteList[10]->Blit(
	//			map->closed_nodes_map[index]->x * TILE_WIDTH,
	//			map->closed_nodes_map[index]->y * TILE_HEIGHT);
	//	}

	//undraw the camera so the font doesn't move out of the screen
	camera->UnDraw();

	//Draw font
	drawFont();

	//quantize cursor
	float x = ((int)(cursor.x + offX) / TILE_WIDTH) * TILE_WIDTH;
	float y = ((int)(cursor.y + offY) / TILE_HEIGHT) * TILE_HEIGHT;
	//draw cursor
	spriteList[(int)editBrick]->Blit(x, y);

}

void drawMap()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void drawMap()
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Draw the map.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


	//draw based on the height of the screen and an extra 100 more to 
	int y = (map->rover.y - MAX_DRAW_Y < 0) ? 0 : map->rover.y - MAX_DRAW_Y;
	int maxY = (map->rover.y + MAX_DRAW_Y > map->mazHeight) ? map->mazHeight : map->rover.y + MAX_DRAW_Y;

	for (; y < maxY; y++)
	{
		//draw based on the width of the screen and an extra 100 more to 
		int x = (map->rover.x - MAX_DRAW_X < 0) ? 0 : map->rover.x - MAX_DRAW_X;
		int maxX = (map->rover.x + MAX_DRAW_X > map->mazWidth) ? map->mazWidth : map->rover.x + MAX_DRAW_X;

		for (; x < maxX; x++)
		{
			//don't draw any thing in the passed the tile size
			if (y >= map->map.size() || (x >= map->map[0].size()))
			{
				//spriteList[0]->Blit(x * TILE_WIDTH, y * TILE_HEIGHT);
			}
			else
			{
				//draw a the tiles
				if (NULL == map->map[y][x] || NULL == (int)map->map[y][x]->typeID)
				{
					spriteList[(int)TileEnum::UNTRAVERSABLE]->Blit(x * TILE_WIDTH, y * TILE_HEIGHT);
				}
				else
				{
					spriteList[(int)(map->map[y][x]->typeID)]->Blit(x * TILE_WIDTH, y * TILE_HEIGHT);
				}
			}
		}
	}

	//draw the exit
	if (map->exit.x != -1 && map->exit.y != -1)
	{
		spriteList[(int)TileEnum::END]->Blit(map->exit.x * TILE_WIDTH, map->exit.y * TILE_HEIGHT);
	}

	//draw the rover
	if (map->rover.x != -1 && map->rover.y != -1)
	{
		spriteList[(int)TileEnum::ROVER]->Blit(map->rover.x * TILE_WIDTH, map->rover.y * TILE_HEIGHT);
	}
	//printf("Y = %d\n", y);
	//printf("X = %d\n", x);
	//printf("===============\n");

}

void drawFont()
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void drawFont()
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Draw font.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	font->BlitText(50.f, 1040.f, infoTxt);
}

void DoInput(int key, int scancode, int action, int mods)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void DoInput(int key, int scancode, int action, int mods)
	//
	// Method parameters	:	key      - key that was pressed
	//							scancode - the scanned code out of the device
	//							action   - the action on the key
	//							mods     - the moder 
	//
	// Method return		:	void
	//
	// Synopsis				:   Do the user input from the keyboard
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		//save!
		map->SaveMap("level.txt");
	}

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		//load!
		map->LoadMap("Alex_test_map1024.txt");
	}
	if (key == GLFW_KEY_E && action == GLFW_RELEASE)
	{
		//load!
		map->LoadMap("Alex_test_map2.txt");
	}
	if (key == GLFW_KEY_R && action == GLFW_RELEASE)
	{
		//load!
		map->LoadMap("Alex_test_map3.txt");
	}

	if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
	{
		solveMap();
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		editBrick = TileEnum::UNTRAVERSABLE;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		editBrick = TileEnum::TRAVERSABLE;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		editBrick = TileEnum::END;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		editBrick = TileEnum::ROVER;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		editBrick = TileEnum::UNKNOWN;

	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		camera->Pan(1, 0);
		(offX + PAN_TILES_BY >= map->mazWidth) ?
			offX = 0 :
			offX += PAN_TILES_BY;
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		camera->Pan(-1, 0);
		(offX - PAN_TILES_BY < 0) ?
			offX = 0 :
			offX -= PAN_TILES_BY;
	}

	if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && action == GLFW_RELEASE)
	{
		camera->Pan(0, 0);
	}

	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		camera->Pan(0, 1);
		(offY + PAN_TILES_BY >= map->mazHeight) ?
			offY = 0 :
			offY += PAN_TILES_BY;
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS))
	{
		camera->Pan(0, -1);
		(offY - PAN_TILES_BY < 0) ?
			offY = 0 :
			offY -= PAN_TILES_BY;
	}

	if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_UP) && action == GLFW_RELEASE)
	{
		camera->Pan(0, 0);
	}
}

void DoCursor(double x, double y)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void DoCursor(double x, double y)
	//
	// Method parameters	:	x - the cursor x position on the screen
	//							y - the cursor y position on the screen
	//
	// Method return		:	void
	//
	// Synopsis				:   get the cursor x y position and store them in global variables.
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//demonstrates how to convert the raw cursor coords passed from the OS
	//into Blit3D screenspace coords
	cursor.x = x * (blit3D->screenWidth / blit3D->trueScreenWidth);
	cursor.y = (blit3D->trueScreenHeight - y) * (blit3D->screenHeight / blit3D->trueScreenHeight);
}

void DoMouseButton(int button, int action, int mods)
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	void DoMouseButton(int button, int action, int mods)
	//
	// Method parameters	:	button   - the mouse button that was clicked
	//							action   - the action on the key
	//							mods     - the moder 
	//
	// Method return		:	void
	//
	// Synopsis				:   get the mouse actions
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//quantize cursor
		float x = ((int)cursor.x / TILE_WIDTH) * TILE_WIDTH;
		float y = ((int)cursor.y / TILE_HEIGHT)* TILE_HEIGHT;
		if ((x >= 0 || x < map->mazWidth) && (y >= 0 || y < map->mazHeight))
			map->map[y][x]->typeID = TileEnum::UNTRAVERSABLE;
		//remove any brick under us
		//for (int b = brickList.size() - 1; b >= 0; --b)
		//{
		//	if (x == brickList[b]->x && y == brickList[b]->y)
		//	{
		//		delete brickList[b];
		//		brickList.erase(brickList.begin() + b);
		//	}
		//}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//quantize cursor
		float x = ((int)cursor.x / TILE_WIDTH);
		float y = ((int)cursor.y / TILE_HEIGHT);
		//printf("cursor.x = %.2f", x);
		//printf("cursor.y = %.2f", y);
		/*if ((x >= 0 || x < map->mazWidth) && (y >= 0 || y < map->mazHeight))
			map->tileList[y][x]->typeID = TileEnum::UNTRAVERSABLE;*/
			//remove any brick under us
			//for (int b = brickList.size() - 1; b >= 0; --b)
			//{
			//	if (x == brickList[b]->x && y == brickList[b]->y)
			//	{
			//		delete brickList[b];
			//		brickList.erase(brickList.begin() + b);
			//	}
			//}
		if ((x >= 0 || x < map->mazWidth) && (y >= 0 || y < map->mazHeight))
			map->map[y][x]->typeID = editBrick;
		//add new brick
		//Tile *b = new Tile;
		//b->x = x;
		//b->y = y;
		//b->typeID = (TileEnum)editBrick;
		//brickList.push_back(b);
	}
}

int main(int argc, char *argv[])
{
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Method				:	int main(int argc, char *argv[])
	//
	// Method parameters	:	
	//
	// Method return		:	void
	//
	// Synopsis				:   Build and run Blit3D
	//							
	//
	// Modifications		:
	//								Date			 Developer				   Notes
	//								----			 ---------			 	   -----
	//							Oct 5, 2018		  Mohammed Al-Safwan		Initial setup
	//
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, SCREEN_WIDTH, SCREEN_HEIGHT);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);
	blit3D->SetDoCursor(DoCursor);
	blit3D->SetDoMouseButton(DoMouseButton);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::MULTITHREADED);
	if (blit3D) delete blit3D;
}