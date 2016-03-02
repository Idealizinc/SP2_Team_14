/****************************************************************************/
/*!
\file Camera.cpp
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
.cpp file that defines camera
*/
/****************************************************************************/
#include "Camera.h"
#include "Application.h"
#include "Mtx44.h"

/****************************************************************************/
/*!
\brief
Constructor for Camera
\param N/A
\exception N/A
\return N/A
*/
/****************************************************************************/

Camera::Camera()
{
	Reset();
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

Camera::~Camera()
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
\exception N/A
\return
Resulting normalized vectors
*/
/****************************************************************************/

void Camera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = pos;
	this->target = target;
	this->up = up;
}

/****************************************************************************/
/*!
\brief
Sets the minimum and maximum x,y and z boundaries.
\param
		position - Vector3 coordinates for player's position
		target - Vector3 coordinates that determines the direction the player faces
		up - up vector used for calculation of other vectors
\exception N/A
\return
Default Vector3 values
*/
/****************************************************************************/

void Camera::Reset()
{
	position.Set(1, 0, 0);
	target.Set(0, 0, 0);
	up.Set(0, 1, 0);
}

void Camera::Update(double dt)
{
	static const float CAMERA_SPEED = 20.f;
	if(Application::IsKeyPressed(VK_LEFT) || Application::IsKeyPressed('A'))
	{
		position.x -= (float)(CAMERA_SPEED * 0.2 * dt);
	}
	if(Application::IsKeyPressed(VK_RIGHT) || Application::IsKeyPressed('D'))
	{
		position.x += (float)(CAMERA_SPEED * 0.2 * dt);
	}
	if(Application::IsKeyPressed(VK_UP) || Application::IsKeyPressed('W'))
	{
		position.y += (float)(CAMERA_SPEED * 0.2 * dt);
	}
	if(Application::IsKeyPressed(VK_DOWN) || Application::IsKeyPressed('S'))
	{
		position.y -= (float)(CAMERA_SPEED * 0.2 * dt);
	}
}