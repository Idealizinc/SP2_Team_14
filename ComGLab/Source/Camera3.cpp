#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"

#include "MyMath.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up, int mass)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();

	CameraXrotation = 0;
	CameraYrotation = 0;
	maxCameraXrotation = 89;
	minCameraXrotation = -89;

	rotate_Sensitivity = 2;
	cameraMass = 1;
	outOfBounds = false;
	initBoundVec();
}

void Camera3::Update(double dt)
{

	cameraMovement2(dt);
	outOfBounds = false;
	rotateCamera(dt);
}

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

void Camera3::rotateCamera(double dt)
{
	//Lock the cursor's position to the center of the screen.
	int moveX = S_Width / 2;
	int moveY = S_Height / 2;
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	SetCursorPos(moveX, moveY);
	CameraYrotation -= ((mousePosition.x - moveX) * (float)(dt)* rotate_Sensitivity);
	CameraXrotation += ((mousePosition.y - moveY) * (float)(dt)* rotate_Sensitivity);
	CameraXrotation = Math::Clamp(CameraXrotation, minCameraXrotation, maxCameraXrotation);

	target = Vector3(sin(Math::DegreeToRadian(CameraYrotation)) * cos(Math::DegreeToRadian(CameraXrotation)) + position.x,
		-sin(Math::DegreeToRadian(CameraXrotation)) + position.y,
		cos(Math::DegreeToRadian(CameraYrotation)) * cos(Math::DegreeToRadian(CameraXrotation)) + position.z);
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(defaultUp);
	up = right.Cross(view);
}

bool Camera3::boundsCheck2D(const int& xBound, const int& zBound, float& currXPos, float& currZPos)
{
	float pushBackValue = 0.001f;
	float offset = 1 / walkingSpeed;
	float xCopy, zCopy;
	xCopy = currXPos;
	zCopy = currZPos;
	//bound checks
	if (xBound < xCopy + offset)
	{
		currXPos -= pushBackValue;
		return false;
	}
	else if (zBound < zCopy + offset)
	{
		currZPos -= pushBackValue;
		return false;
	}
	else if (-xBound > xCopy - offset)
	{
		currXPos += pushBackValue;
		return false;
	}
	else if (-zBound > zCopy - offset)
	{
		currZPos += pushBackValue;
		return false;
	}
	//still within bounds
	else //if (currXPos <= xBound && currZPos <= zBound && currXPos >= -xBound && currZPos >= -zBound)
	{
		//outOfBounds = false;
		return true;
	}
}

void Camera3::cameraMovement(double dt)
{
	float offsetX, offsetZ;
	if (Application::IsKeyPressed('W') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('A') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('S') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('D') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
	}
}

Vector3 Camera3::getCameraPosition()
{
	return position;
}

bool Camera3::isOutofBounds()
{
	return outOfBounds;
}

void Camera3::cameraMovement2(double dt)
{

	// Movement system, 2.0 where bounds checking is done within the move function.
	// Movement increments
	float walkingX = 0;
	float walkingZ = 0;
	float walkingY = 0;
	if (Application::IsKeyPressed(VK_LSHIFT) || Application::IsKeyPressed(VK_RSHIFT))
	{
		walkingSpeed = 0.5;
	}
	else walkingSpeed = 5;

	if (Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
	}
	if (Application::IsKeyPressed(' ') || Application::IsKeyPressed(VK_SPACE))
	{
		//walkingY += 
	}
<<<<<<< HEAD
	if (walkingX != 0 && northwall1.BoundaryCheck(walkingX + position.x, position.z) && northwall2.BoundaryCheck(walkingX + position.x, position.z) && westwall1.BoundaryCheck(walkingX + position.x, position.z) && westwall2.BoundaryCheck(walkingX + position.x, position.z) && eastwall1.BoundaryCheck(walkingX + position.x, position.z) && eastwall2.BoundaryCheck(walkingX + position.x, position.z) && southwall1.BoundaryCheck(walkingX + position.x, position.z) && southwall2.BoundaryCheck(walkingX + position.x, position.z) && corebase.BoundaryCheck(walkingX + position.x, position.z))
	{
		position.x += walkingX;
	}
	if (walkingZ != 0 && northwall1.BoundaryCheck(position.x, position.z + walkingZ) && northwall2.BoundaryCheck(position.x, position.z + walkingZ) && westwall1.BoundaryCheck(position.x, position.z + walkingZ) && westwall2.BoundaryCheck(position.x, position.z + walkingZ) && eastwall1.BoundaryCheck(position.x, position.z + walkingZ) && eastwall2.BoundaryCheck(position.x, position.z + walkingZ) && southwall1.BoundaryCheck(position.x, position.z + walkingZ) && southwall1.BoundaryCheck(position.x, position.z + walkingZ) && corebase.BoundaryCheck(position.x, position.z + walkingZ))
=======
	if (walkingX != 0 && WorldBack.BoundaryCheck(walkingX + position.x, position.z) && WorldFront.BoundaryCheck(walkingX + position.x, position.z)
		&& WorldLeft.BoundaryCheck(walkingX + position.x, position.z) && WorldRight.BoundaryCheck(walkingX + position.x, position.z)
		&& WorldTop.BoundaryCheck(walkingX + position.x, position.z) && WorldBot.BoundaryCheck(walkingX + position.x, position.z)
		&& Floor2Back.BoundaryCheck(walkingX + position.x, position.z) && Floor2Front.BoundaryCheck(walkingX + position.x, position.z)
		&& Floor2Left.BoundaryCheck(walkingX + position.x, position.z) && Floor2Right.BoundaryCheck(walkingX + position.x, position.z)
		&& Floor2Top.BoundaryCheck(walkingX + position.x, position.z) && Floor2Bot.BoundaryCheck(walkingX + position.x, position.z))
	{
		position.x += walkingX;
	}
	if (walkingZ != 0 && WorldBack.BoundaryCheck(position.x, position.z + walkingZ) && WorldFront.BoundaryCheck(position.x, position.z + walkingZ)
		&& WorldLeft.BoundaryCheck(position.x, position.z + walkingZ) && WorldRight.BoundaryCheck(position.x, position.z + walkingZ)
		&& WorldTop.BoundaryCheck(position.x, position.z + walkingZ) && WorldBot.BoundaryCheck(position.x, position.z + walkingZ)
		&& Floor2Back.BoundaryCheck(position.x, position.z + walkingZ) && Floor2Front.BoundaryCheck(position.x, position.z + walkingZ)
		&& Floor2Left.BoundaryCheck(position.x, position.z + walkingZ) && Floor2Right.BoundaryCheck(position.x, position.z + walkingZ)
		&& Floor2Top.BoundaryCheck(position.x, position.z + walkingZ) && Floor2Bot.BoundaryCheck(position.x, position.z + walkingZ))
>>>>>>> e27f3d3092bf545e5d1f11e68482a300b5973bdf
	{
		position.z += walkingZ;
	}
	



}

void Camera3::initBoundVec()
{
<<<<<<< HEAD
	
	/*min =  trans - scale/2 ;
	max =  trans + scale/2;*/
	northwall1.set(15.0,18.8,4.6,18.0);
	southwall1.set(-18.3,-16.1, -18.0,-4.5);
	eastwall1.set(-17.9,-4.6,16.4,19.0);
	westwall1.set(4.5,18.1,-19.3, -16.1);
	northwall2.set(15.6, 18.8, -18.0,-4.6);
	southwall2.set(-18.3,-15.9,4.6,18.0);
	eastwall2.set(4.6,18.0,15.3,19.0);
	westwall2.set(-18.0,-4.6,-19.3,-14.8);

	northgate1.set();
	southgate1.set();
	eastgate1.set();
	westgate1.set();
	northgate2.set();
	southgate2.set();
	eastgate2.set();
	westgate2.set();

	corebase.set(-1.3, 1.1, -1.4, 3.1);

	
=======
	/*Note
	min =  trans - scale/2 ;
	max =  trans + scale/2;
	*/
	/*BoatBack.set(-150, 150, -160, -150);
	BoatFront.set(-150, 150, 150, 160);*/
	WorldBot.set(-150, 150, -150, 150, -5, 0);
	WorldTop.set(-150, 150, -150, 150, 30, 40);
	WorldFront.set(-150, 150, 150, 160, -5, 30);
	WorldBack.set(-150, 150, -160, -150, -5, 30);
	WorldLeft.set(150, 160, -150, 150, -5, 30);
	WorldRight.set(-160, -150, -150, 150, -5, 30);
	Floor2Left.set(11, 13, -17, 17, 10, 17);
	Floor2Right.set(-13, 11, -17, 17, 10, 17);
	Floor2Front.set(-11, 11, 15, 17, 10, 17);
	Floor2Back.set(-11, 11, -17, -15, 10, 17);
	Floor2Top.set(-11, 11, -17, 17, 15, 17);
	Floor2Bot.set(-11, 11, -17, 17, 8, 10);
>>>>>>> e27f3d3092bf545e5d1f11e68482a300b5973bdf
}


