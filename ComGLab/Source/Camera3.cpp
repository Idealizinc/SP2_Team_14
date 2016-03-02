/****************************************************************************/
/*!
\file Camera3.cpp
\author Ryan Lim Rui An, Teh Kai Hong Bryan
\par email: 150577L@mymail.nyp.edu.sg, 150587A@mymail.nyp.edu.sg
\brief
.cpp file that defines camera
*/
/****************************************************************************/

#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"
#include "CameraPhysics.h"
#include "MyMath.h"

/****************************************************************************/
/*!
\brief
Constructor for Camera
\param N/A
\exception N/A
\return N/A
*/
/****************************************************************************/

Camera3::Camera3()
{
}

/****************************************************************************/
/*!
\brief
Destructor for Camera
\param N/A
\exception N/A
\return N/A
*/
/****************************************************************************/

Camera3::~Camera3()
{
}

/****************************************************************************/
/*!
\brief
Initializes variables
\param
		pos - Vector3 coordinates for player's position
		target - Vector3 coordinates that determines the direction the player faces
		up - up vector used for calculation of other vectors
		mass - defines the player's mass
		CameraXrotation - self explanatory
		CameraYrotation - self explanatory
		minCameraXrotation - Minimum camera rotation on the x-axis
		maxCameraXrotation - Maximum camera rotation on the x-axis
		minCameraYrotation - Minimum camera rotation on the y-axis
		maxCameraYrotation - Maximum camera rotation on the y-axis
		CanMoveCamera - boolean that determines whether the camera can be moved
		rotate_Sensitivity - value that adjusts mouse sensitivity
		teleCheck - boolean that checks if the player is standing on the teleporters
		shipCheck - boolean that checks if the player is standing close to the drivable ship
		coreCheck - boolean that checks if the player is standing close to the base core
		isGravityOn - boolean that determines if Gravity is toggled on or off
		isMaxJump - boolean that checks if the player has reached max jump height
		isJump - boolean that checks if the player is jumping
		maxJump - value that determines the max jump height
		maxJump2 - value that determines the max jump height on the second floor
		level - value that determines what floor the player is on
\exception N/A
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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
	maxCameraYrotation = 360;
	minCameraYrotation = -360;

	CanMoveCamera = true;

	rotate_Sensitivity = 2;
	initBoundVec();

	teleCheck = false;
	shipCheck = false;
	coreCheck = false;

	isGravityOn = true;
	isMaxJump = false;
	isJump = false;
	maxJump = 8.0f;
	maxJump2 = 16.5f;
	level = 1;
}

/****************************************************************************/
/*!
\brief
Method where values are changed
\param
		teleCheck - boolean that checks if the player is standing on the teleporters
		shipCheck - boolean that checks if the player is standing close to the drivable ship
		coreCheck - boolean that checks if the player is standing close to the base core
\exception N/A
\return
Resulting boolean values
*/
/****************************************************************************/

void Camera3::Update(double dt)
{

	cameraMovement2(dt);
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
}

/****************************************************************************/
/*!
\brief
Resets camera position to default value
\param
		pos - Vector3 coordinates for player's position
		target - Vector3 coordinates that determines the direction the player faces
		up - up vector used for calculation of other vectors
\exception N/A
\return
Resulting normalized vectors
*/
/****************************************************************************/

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

/****************************************************************************/
/*!
\brief
Rotates the camera
\param
		moveX - value that determines camera movement on the x-axis
		moveY - value that determines camera movement on the y-axis
		S_Width - value that determines the screen width
		S_Height - value that determines the screen height
		CameraXrotation - self explanatory
		CameraYrotation - self explanatory
		minCameraXrotation - Minimum camera rotation on the x-axis
		maxCameraXrotation - Maximum camera rotation on the x-axis
		minCameraYrotation - Minimum camera rotation on the y-axis
		maxCameraYrotation - Maximum camera rotation on the y-axis
		pos - Vector3 coordinates for player's position
		target - Vector3 coordinates that determines the direction the player faces
		up - up vector used for calculation of other vectors
\exception N/A
\return
Resulting normalized vectors
*/
/****************************************************************************/

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
	if (maxCameraYrotation < 360 && minCameraYrotation < 360)
	{
		CameraYrotation = Math::Clamp(CameraYrotation, minCameraYrotation, maxCameraYrotation);
	}
	target = Vector3(sin(Math::DegreeToRadian(CameraYrotation)) * cos(Math::DegreeToRadian(CameraXrotation)) + position.x,
		-sin(Math::DegreeToRadian(CameraXrotation)) + position.y,
		cos(Math::DegreeToRadian(CameraYrotation)) * cos(Math::DegreeToRadian(CameraXrotation)) + position.z);
	view = (target - position).Normalized();
	Vector3 right = view.Cross(defaultUp);
	up = right.Cross(view);
}

/****************************************************************************/
/*!
\brief
Calls the value for the Look Vector
\param
		view - Vector3 value for Look Vector
\return
view
*/
/****************************************************************************/

Vector3 Camera3::getLookVector()
{
	return view;
}

/****************************************************************************/
/*!
\brief
Calls the value for current position
\param
		position - Vector3 value for current position
\return
view
*/
/****************************************************************************/

Vector3 Camera3::getCameraPosition()
{
	return position;
}

/****************************************************************************/
/*!
\brief
Method for player movement
\param
		walkingX - value for modifying walking speed along the x-axis
		walkingZ - value for modifying walking speed along the z-axis
		walkingY - value for modifying walking speed along the y-axis
		gravity - value for gravity
		spaceModeOn - boolean that determines which map is rendered on screen
		CanMoveCamera - boolean that determines whether the camera can be moved
		isGravityOn - boolean that determines if Gravity is toggled on or off
		isJump - boolean that checks if the player is jumping
		position - Vector3 value for current position
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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
	
	if (!spaceModeOn && Application::IsKeyPressed(VK_LSHIFT) || Application::IsKeyPressed(VK_RSHIFT))
	{
		walkingSpeed = 2;
	}
	else walkingSpeed = baseWalkSpeed;

	if (spaceModeOn)
	{
		if (CanMoveCamera && Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
		{
			walkingY += (float)(sin(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
			walkingY += (float)(cos(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
		}
		if (CanMoveCamera && Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
		{
			walkingY += (float)(sin(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
			walkingY += (float)(cos(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
		}
	}
	else{
		if (CanMoveCamera && Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
		{
			walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
			walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
		}
		if (CanMoveCamera && Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
		{
			walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
			walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
		}
	}
	if (CanMoveCamera && Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
	}
	if (CanMoveCamera && Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
	}
	if (!spaceModeOn && Application::IsKeyPressed(' '))
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
	if (spaceModeOn == false){
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
		//Gravity
		if (isGravityOn == true)
		{
			if (/*velocityY != 0 &&*/ northwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && northwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
				&& westwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && westwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity)
				&& eastwall1.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) && eastwall2.BoundaryCheck(position.x, position.z, (position.y - 5) - gravity) 
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
	else
	{
		if (walkingX != 0
			&& WorldBack.BoundaryCheck(walkingX + position.x, position.z, position.y) && WorldFront.BoundaryCheck(walkingX + position.x, position.z, position.y)
			&& WorldLeft.BoundaryCheck(walkingX + position.x, position.z, position.y) && WorldRight.BoundaryCheck(walkingX + position.x, position.z, position.y)
			&& WorldTop.BoundaryCheck(walkingX + position.x, position.z, position.y) && WorldBot.BoundaryCheck(walkingX + position.x, position.z, position.y))
		{
			position.x += walkingX;
		}
		if (walkingY != 0
			&& WorldBack.BoundaryCheck(position.x, position.z, position.y + walkingY) && WorldFront.BoundaryCheck(position.x, position.z, position.y + walkingY)
			&& WorldLeft.BoundaryCheck(position.x, position.z, position.y + walkingY) && WorldRight.BoundaryCheck(position.x, position.z, position.y + walkingY)
			&& WorldTop.BoundaryCheck(position.x, position.z, position.y + walkingY) && WorldBot.BoundaryCheck(position.x, position.z, position.y + walkingY))
		{
			position.y += walkingY;
		}
	}
}

/****************************************************************************/
/*!
\brief
Initializes Boundary values
\param
		northwall1 - Boundary for the wall facing the north on the first floor of the base
		southwall1 - Boundary for the wall facing the south on the first floor of the base
		eastwall1 - Boundary for the wall facing the east on the first floor of the base
		westwall1 - Boundary for the wall facing the west on the first floor of the base
		northwall2 - Boundary for the wall facing the north on the first floor of the base
		southwall2 - Boundary for the wall facing the south on the first floor of the base
		eastwall2 - Boundary for the wall facing the east on the first floor of the base
		westwall2 - Boundary for the wall facing the west on the first floor of the base
		corebase - Boundary for the base core
		WorldBot - Boundary for the floor of the world
		WorldTop - Boundary for the top of the world
		WorldFront - Boundary for the wall at the north of the world
		WorldBack - Boundary for the wall at the south of the world
		WorldLeft - Boundary for the wall at the west of the world
		WorldRight - Boundary for the wall at the eart of the world
		Floor2WestLeft - Boundary for the left wall facing the west on the second floor of the base
		Floor2WestRight - Boundary for the right wall facing the west on the second floor of the base
		Floor2EastLeft - Boundary for the left wall facing the east on the second floor of the base
		Floor2EastRight - Boundary for the right wall facing the east on the second floor of the base
		Floor2FrontLeft - Boundary for the left wall facing the north on the second floor of the base
		Floor2FrontRight - Boundary for the right wall facing the north on the second floor of the base
		Floor2BackLeft - Boundary for the left wall facing the south on the second floor of the base
		Floor2BackRight - Boundary for the right wall facing the south on the second floor of the base
		Floor2Top - Boundary for the ceiling of the second floor of the base
		Floor2Bot - Boundary for the floor of the second floor of the base
		TeleporterF1NW - Boundary for the teleporter in the north west corner of the first floor of the base
		TeleporterF1NE - Boundary for the teleporter in the north east corner of the first floor of the base
		TeleporterF1SW - Boundary for the teleporter in the south west corner of the first floor of the base
		TeleporterF1SE - Boundary for the teleporter in the south earth corner of the first floor of the base
		ShipCheck - Boundary for the area around the drivable ship
		CoreCheck - Boundary for the area around the core in the middle of the base
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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

	TeleporterF1NW.set(10, 17, 9, 17, -5, 10);
	TeleporterF1NE.set(-17, -10, 9, 17, -5, 10);
	TeleporterF1SW.set(10, 17, -17, -9, -5, 10);
	TeleporterF1SE.set(-17, -10, -17, -9, -5, 10);

	ShipCheck.set(-25, 15, 54, 96, 0, 10);
	CoreCheck.set(-2.5, 2.5, -2.5, 2.5, 0, 10);
}

/****************************************************************************/
/*!
\brief
Method that allows the player to jump
\param
		isGravityOn - boolean that determines if Gravity is toggled on or off
		isMaxJump - boolean that checks if the player has reached max jump height
		isJump - boolean that checks if the player is jumping
		maxJump - value that determines the max jump height
		maxJump2 - value that determines the max jump height on the second floor
		level - value that determines what floor the player is on
		position - Vector3 value for current position
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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
			position.y += (float)(5 * dt);
		}
		else
		{
			position.y -= (float)(5 * dt);
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