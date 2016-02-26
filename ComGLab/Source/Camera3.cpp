#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"
#include "CameraPhysics.h"
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
	cameraMass = mass;
	outOfBounds = false;
	initBoundVec();
	teleCheck = false;
	leftGateA = -2.3;
	leftGateB = 2.3;
	rightGateA = 2.3;
	rightGateB = -2.3;
	frontGateA = 2.3;
	frontGateB = -2.3;
	backGateA = 2.3;
	backGateB = -2.3;
	checkLeftGate = false;
	openLeftGate = false;
	checkRightGate = false;
	openRightGate = false;
	checkFrontGate = false;
	openFrontGate = false;
	checkBackGate = false;
	openBackGate = false;

	shipCheck = false;

	jumpForce = 0;
	groundlevel = 6;
	jumpImpulse = 0;

	isGravityOn = true;
	isMaxJump = false;
	isJump = false;
	maxJump = 8.0f;
	maxJump2 = 16.5f;
	level = 1;
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
	
	if (!ShipCheck.BoundaryCheck(position.x, position.z, position.y))
	{
		shipCheck = true;
	}
	else
	{
		shipCheck = false;
	}

	if (!CoreCheck.BoundaryCheck(position.x, position.z, position.y))
	{
		coreCheck = true;
	}
	else
	{
		coreCheck = false;
	}

	if (!leftGate.BoundaryCheck(position.x, position.z, position.y))
	{
		checkLeftGate = true;
	}
	else
	{
		checkLeftGate = false;
	}
	if (openLeftGate == true)
	{
		leftGateA -= 1;
		leftGateB += 1;
	}
	if (leftGateA <= -5.3 && leftGateB >= 5.3)
	{
		openLeftGate = false;
	}
	if (openRightGate == true)
	{
		rightGateA += 1;
		rightGateB -= 1;
	}
	if (rightGateA >= 5.3 && rightGateB <= -5.3)
	{
		openRightGate = false;
	}
	if (openFrontGate == true)
	{
		frontGateA += 1;
		frontGateB -= 1;
	}
	if (frontGateA >= 5.3 && frontGateB <= -5.3)
	{
		openFrontGate = false;
	}
	if (openBackGate == true)
	{
		backGateA += 1;
		backGateB -= 1;
	}
	if (backGateA >= 5.3 && backGateB <= -5.3)
	{
		openBackGate = false;
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
	float gravity = 9.8 * dt;
	//velocityY += gravity * dt;
	//walkingY = velocityY * dt;
	
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
	//if (Application::IsKeyPressed(' ') || Application::IsKeyPressed(VK_SPACE))
	//{
		//position.y -= velocityY * 0.5;
		//velocityY -= gravity;
	//}
	//if (Application::IsKeyPressed(VK_SPACE))
	//{
		/*if (position.y <= groundlevel)
		{*/
			/*jumpImpulse = 2 * dt;
			jumpForce = 15000;
			jumpSpeed = static_cast<float>(Physics::getAcceleration(jumpForce, cameraMass));
			position.y += jumpSpeed * (float)jumpImpulse;*/
		//}
	//}
	/*if (position.y > groundlevel)
	{
		jumpSpeed = Physics::gravitational_pulled(jumpSpeed, dt);
		position.y += jumpSpeed * (float)dt;
	}*/
	if (Application::IsKeyPressed(' '))
	{
		isGravityOn = false;
		isJump = true;

		if (position.y <= 10.0f)
			level = 1;
		else if (position.y >= 15.0f)
			level = 2;
	}

	if (isJump == true)
	{
		jump(dt, level);
	}

	rotateCamera(dt);
	/*if (Application::IsKeyPressed('R'))
	{
	    Reset();
	}*/
	if (walkingX != 0 && northwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && northwall2.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& westwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && westwall2.BoundaryCheck(walkingX + position.x, position.z, position.y)
		&& eastwall1.BoundaryCheck(walkingX + position.x, position.z, position.y) && eastwall2.BoundaryCheck(walkingX + position.x, position.z, position.y) && PlayerShip.BoundaryCheck(walkingX + position.x, position.z, position.y)
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
		&& eastwall1.BoundaryCheck(position.x, position.z + walkingZ, position.y) && eastwall2.BoundaryCheck(position.x, position.z + walkingZ, position.y) && PlayerShip.BoundaryCheck(walkingX, position.z + walkingZ, position.y)
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
	//Gravity
	if (isGravityOn == true)
	{
		if (/*velocityY != 0 &&*/ northwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && northwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
			&& westwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && westwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
			&& eastwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && eastwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && PlayerShip.BoundaryCheck(walkingX, position.z, (position.y - 5) - gravity)
			&& southwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && southwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && corebase.BoundaryCheck(walkingX, position.z, (position.y - 5) - gravity)
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

	corebase.set(-1.5, 1.5, -1.5, 1.5, -5, 10);
	
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

	Base.set(-18, 18, -19, 19, -5, 10);

	//leftRock1.set(-44.9, -55.6, -2.8, -12.5, 0, 8);
	//leftRock2.set(-80.4, -109.4, -17.2, -46.7, 0, 8);
	//leftRock3.set(-92.4, -130, -105.6, -138.9, 0, 8);
	//frontRock1.set(4.9, -5.7, -53.6, -66, 0, 8);
	//frontRock2.set(53.5, 66.9, -110, -128.3, 0, 8);
	//rightRock1.set(44.8, 55.2, 5.4, -5.4, 0, 8);
	//rightRock2.set(95.3, 123.5, -14.7, -44.7, 0, 8);
	//backRock1.set(-4.8, 5.2, 54.2, 65.7, 0, 8);
	//backRock2.set(18.1, 34.1, 117.1, 144, 0, 8);
	//backRock3.set(-150, -140, 116.8, 144.4, 0, 8);

	TeleporterF1NW.set(10, 17, 9, 17, -5, 10);
	TeleporterF1NE.set(-17, -10, 9, 17, -5, 10);
	TeleporterF1SW.set(10, 17, -17, -9, -5, 10);
	TeleporterF1SE.set(-17, -10, -17, -9, -5, 10);

	//PlayerShip.set(-18, 10, 60, 88, 0, 10);
	ShipCheck.set(-25, 15, 54, 96, 0, 10);
	CoreCheck.set(-2.5, 2.5, -2.5, 2.5, 0, 10);
}

void Camera3::jump(double dt, int level)
{
	if (level == 1)
	{
		if (isMaxJump == false) 
		{
			position.y += (float)(2 * dt);
		}
		else
		{
			position.y -= (float)(2 * dt);
		}
		if (position.y > maxJump)
		{
			isMaxJump = true;
			position.y = maxJump;
		}
		else if (position.y < 5.0f)
		{
			position.y = 5.0f;
			isMaxJump = false;
			isJump = false;
			isGravityOn = true;
		}
	}
	else if (level == 2)
	{
		if (isMaxJump == false) 
		{
			position.y += (float)(4 * dt);
		}
		else
		{
			position.y -= (float)(4 * dt);
		}
		if (position.y > maxJump2)
		{
			isMaxJump = true;
			position.y = maxJump2;
		}
		else if (position.y < 15.0f)
		{
			position.y = 15.0f;
			isMaxJump = false;
			isJump = false;
			isGravityOn = true;
		}
	}
}


