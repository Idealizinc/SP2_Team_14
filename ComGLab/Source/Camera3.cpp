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
	teleCheck = false;
}

void Camera3::Update(double dt)
{

	cameraMovement2(dt);
	outOfBounds = false;
	rotateCamera(dt);

	if (!TeleporterF1NW.BoundaryCheck(position.x, position.z, position.y) || !TeleporterF1NE.BoundaryCheck(position.x, position.z, position.y)
		|| !TeleporterF1SW.BoundaryCheck(position.x, position.z, position.y) || !TeleporterF1SE.BoundaryCheck(position.x, position.z, position.y))
	{
		teleCheck = true;
	}
	else
	{
		teleCheck = false;
	}
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
	view = (target - position).Normalized();
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

Vector3 Camera3::getLookVector()
{
	return view;
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
	float gravity = dt * 10;
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
	if (walkingX != 0 && northwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && northwall2.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& westwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && westwall2.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& eastwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && eastwall2.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& southwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && southwall2.BoundaryCheck(walkingX + position.x, position.z, position.y) && corebase.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& WorldBack.BoundaryCheck(walkingX + position.x, position.z, position.y) && WorldFront.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& WorldLeft.BoundaryCheck(walkingX + position.x, position.z, position.y) && WorldRight.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& WorldTop.BoundaryCheck(walkingX + position.x, position.z, position.y) && WorldBot.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& Floor2BackLeft.BoundaryCheck(walkingX + position.x, position.z, position.y) && Floor2BackRight.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& Floor2FrontLeft.BoundaryCheck(walkingX + position.x, position.z, position.y) && Floor2FrontRight.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& Floor2WestLeft.BoundaryCheck(walkingX + position.x, position.z, position.y) && Floor2WestRight.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& Floor2EastLeft.BoundaryCheck(walkingX + position.x, position.z, position.y) && Floor2EastRight.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& Floor2Top.BoundaryCheck(walkingX + position.x, position.z, position.y) && Floor2Bot.BoundaryCheck(walkingX + position.x, position.z, position.y))
	{
		position.x += walkingX;
	}
	if (walkingZ != 0 && northwall1.BoundaryCheck(position.x, position.z + walkingZ, position.y) && northwall2.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& westwall1.BoundaryCheck(position.x, position.z + walkingZ, position.y) && westwall2.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& eastwall1.BoundaryCheck(position.x, position.z + walkingZ, position.y) && eastwall2.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& southwall1.BoundaryCheck(position.x, position.z + walkingZ, position.y) && southwall1.BoundaryCheck(position.x, position.z + walkingZ, position.y) && corebase.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& WorldBack.BoundaryCheck(position.x, position.z + walkingZ, position.y) && WorldFront.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& WorldLeft.BoundaryCheck(position.x, position.z + walkingZ, position.y) && WorldRight.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& WorldTop.BoundaryCheck(position.x, position.z + walkingZ, position.y) && WorldBot.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& Floor2BackLeft.BoundaryCheck(position.x, position.z + walkingZ, position.y) && Floor2BackRight.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& Floor2FrontLeft.BoundaryCheck(position.x, position.z + walkingZ, position.y) && Floor2FrontRight.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& Floor2WestLeft.BoundaryCheck(position.x, position.z + walkingZ, position.y) && Floor2WestRight.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& Floor2EastLeft.BoundaryCheck(position.x, position.z + walkingZ, position.y) && Floor2EastRight.BoundaryCheck(position.x, position.z + walkingZ, position.y)
		&& Floor2Top.BoundaryCheck(walkingX, position.z + walkingZ, position.y) && Floor2Bot.BoundaryCheck(walkingX, position.z + walkingZ, position.y))
	{
		position.z += walkingZ;
	}
	if (gravity != 0 && northwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && northwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& westwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && westwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& eastwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && eastwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& southwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && southwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && corebase.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& WorldBack.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && WorldFront.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& WorldLeft.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && WorldRight.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& WorldTop.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && WorldBot.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& Floor2BackLeft.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && Floor2BackRight.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& Floor2FrontLeft.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && Floor2FrontRight.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& Floor2WestLeft.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && Floor2WestRight.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& Floor2EastLeft.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && Floor2EastRight.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
		&& Floor2Top.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && Floor2Bot.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity))
	{
		position.y -= gravity;
	}
	
	//Teleporter
	if ((teleCheck == true && TeleporterF1NW.BoundaryCheck(position.x, position.z, position.y)
		|| (teleCheck == true && TeleporterF1NE.BoundaryCheck(position.x, position.z, position.y))
		|| (teleCheck == true && TeleporterF1SW.BoundaryCheck(position.x, position.z, position.y))
		|| (teleCheck == true && TeleporterF1SE.BoundaryCheck(position.x, position.z, position.y))) && Application::IsKeyPressed('E'))
	{
		position.x = 5;
		position.y = 16;
		position.z = 3;
	}
}

void Camera3::initBoundVec()
{	
	/*min =  trans - scale/2 ;
	max =  trans + scale/2;*/
	northwall1.set(15.0, 18.8, 4.6, 18.0, -5, 10);
	southwall1.set(-18.3, -16.1, -18.0, -4.5, -5, 10);
	eastwall1.set(-17.9, -4.6, 16.4, 19.0, -5, 10);
	westwall1.set(4.5, 18.1, -19.3, -16.1, -5, 10);
	northwall2.set(15.6, 18.8, -18.0, -4.6, -5, 10);
	southwall2.set(-18.3, -15.9, 4.6, 18.0, -5, 10);
	eastwall2.set(4.6, 18.0, 15.3, 19.0, -5, 10);
	westwall2.set(-18.0, -4.6, -19.3, -14.8, -5, 10);

	northgate1.set();
	southgate1.set();
	eastgate1.set();
	westgate1.set();
	northgate2.set();
	southgate2.set();
	eastgate2.set();
	westgate2.set();

	corebase.set(-1.5, 1.5, -1.5, 1.5, -5 , 10);

	WorldBot.set(-150, 150, -150, 150, -5, 0);
	WorldTop.set(-150, 150, -150, 150, 30, 40);
	WorldFront.set(-150, 150, 150, 160, -5, 30);
	WorldBack.set(-150, 150, -160, -150, -5, 30);
	WorldLeft.set(150, 160, -150, 150, -5, 30);
	WorldRight.set(-160, -150, -150, 150, -5, 30);
	Floor2WestLeft.set(12, 14, -14, -3, 10, 23);
	Floor2WestRight.set(12, 14, 3, 14, 10, 23);
	Floor2EastLeft.set(-14, -12, -14, -3, 10, 23);
	Floor2EastRight.set(-14, -12, 3, 14, 10, 23);
	Floor2FrontLeft.set(3, 12, 11.5, 14, 10, 23);
	Floor2FrontRight.set(-12, -3, 11.5, 14, 10, 23);
	Floor2BackLeft.set(3, 12, -14, -11.5, 10, 23);
	Floor2BackRight.set(-12, -3, -14, -11.5, 10, 23);
	Floor2Top.set(-14, 14, -14, 14, 20, 23);
	Floor2Bot.set(-18, 18, -18, 18, 8, 10);

	TeleporterF1NW.set(10, 17, 9, 17, -5, 10);
	TeleporterF1NE.set(-17, -10, 9, 17, -5, 10);
	TeleporterF1SW.set(10, 17, -17, -9, -5, 10);
	TeleporterF1SE.set(-17, -10, -17, -9, -5, 10);
}


