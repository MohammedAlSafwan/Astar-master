#pragma once

/* Example Camera class for panning and screenshake effects*/

#include "Blit3D.h"
#include <atomic>
#include <random>

class Camera2D
{	
private:
	std::mt19937 rng;

	//screenshake variables	
	glm::vec3 shakeOffset; //use this to shake the camera
	float angle;
	float amount;
	bool shaking;
	float shakeTimer;
	glm::vec3 panOffset; //use this to pan the camera around
	glm::vec3 moveDir; //direction to pan in
	float halfScreenWidth, halfScreenHeight;

public:
	std::atomic<int> panX, panY, panZ; //use these to do whole pixel panning

	//set it's valid pan area
	//the center of the camera will be restricted inside these bounds
	float minX, minY, maxX, maxY, minZ, maxZ; //min and max offsets in each direction
	float panSpeed; //how fast we pan in pixels/seconds when using the Pan() method
	int screenOffX = 0;
	int screenOffY = 0;
	//screenshake variables	
	float shakeReductionFactor;//scales the shake amount down every 1/60th of a second

	Camera2D();

	//Pan() moves us in the direction of moveDir,
	//at panSpeed length per second.
	void Pan(float x, float y);

	//PanTo moves us to the pan location provided
	void PanTo(float x, float y);

	void Shake(float radiusInPixels);
	
	void StopShaking();

	void Update(float seconds);

	void Draw();

	void UnDraw();
};