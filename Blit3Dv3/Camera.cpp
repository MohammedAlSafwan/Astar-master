
#include"Camera.h"

extern Blit3D *blit3D;
std::uniform_real_distribution<float> angle_dist(0, M_2_PI); //360 degrees
std::uniform_real_distribution<float> angle_variance_dist(-M_PI / 6.0, M_PI / 6.0); //+-30 degrees

Camera2D::Camera2D()
{
	std::random_device rd;
	rng.seed(rd());

	panOffset = glm::vec3(0, 0, 0);
	minX = minY = minZ = 0;
	maxX = maxY = maxZ = 0;
	moveDir = glm::vec3(0, 0, 0);
	panSpeed = 1000;
	halfScreenWidth = blit3D->screenWidth / 2;
	halfScreenHeight = blit3D->screenHeight / 2;
	shakeOffset = glm::vec3(0, 0, 0);

	shaking = false;
	angle = 0;
	amount = 0;
	shakeTimer = 0;
	shakeReductionFactor = 0.9f;
}

//Pan() moves us in the direction of moveDir,
//at panSpeed length per second.
void Camera2D::Pan(float x, float y)
{
	moveDir.x = x;
	moveDir.y = y;

	if (!(x == 0 && y == 0)) moveDir = glm::normalize(moveDir);
}

//PanTo moves us to the pan location provided
void Camera2D::PanTo(float x, float y)
{
	panOffset.x = x;
	panOffset.y = y;
}

void Camera2D::Shake(float radiusInPixels)
{
	if (radiusInPixels > amount)
		amount = radiusInPixels;

	angle = angle_dist(rng);
	shakeOffset.x = cos(angle);
	shakeOffset.y = sin(angle);
	shakeOffset.z = 0;
	shakeOffset *= amount;
	shaking = true;
}

void Camera2D::StopShaking()
{
	shaking = false;
	shakeOffset = glm::vec3(0.f, 0.f, 0.f);
}

void Camera2D::Update(float seconds)
{
	panOffset += moveDir * panSpeed * seconds;

	//bounds check
	if (panOffset.x < minX) panOffset.x = minX;
	if (panOffset.x > maxX) panOffset.x = maxX;
	if (panOffset.y < minY) panOffset.y = minY;
	if (panOffset.y > maxY) panOffset.y = maxY;
	if (panOffset.z < minZ) panOffset.z = minZ;
	if (panOffset.z > maxZ) panOffset.z = maxZ;

	if (shaking)
	{
		shakeTimer += seconds;
		if (shakeTimer >= 1.f / 60.f)
		{
			shakeTimer = 0;
			amount *= shakeReductionFactor; //reduce amount of shake

			if (amount < 1.f)
			{
				StopShaking();
			}
			else
			{

				angle = angle - M_PI + angle_variance_dist(rng); //shake backwards from last shake
				shakeOffset.x = cos(angle);
				shakeOffset.y = sin(angle);
				shakeOffset.z = 0;
				shakeOffset *= amount;
			}
		}
	}

	//convert to whole pixels
	panX = halfScreenWidth - panOffset.x;
	panY = halfScreenHeight - panOffset.y;
	panZ = panOffset.z;

	//add shake
	if (shaking)
	{
		panX += shakeOffset.x;
		panY += shakeOffset.y;
		panZ += shakeOffset.z;
	}
}

void Camera2D::Draw()
{
	blit3D->viewMatrix = glm::mat4(1.f); //identity matrix
	blit3D->viewMatrix = glm::translate(blit3D->viewMatrix, glm::vec3(panX + screenOffX, panY + screenOffY, panZ)); //translate the view matrix by the offset
	blit3D->shader2d->setUniform("viewMatrix", blit3D->viewMatrix); //send new matrix to the shader
}

void Camera2D::UnDraw()
{
	blit3D->viewMatrix = glm::mat4(1.f); //identity matrix
	blit3D->shader2d->setUniform("viewMatrix", blit3D->viewMatrix); //send new matrix to the shader
}