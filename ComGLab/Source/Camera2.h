/****************************************************************************/
/*!
\file Camera2.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Class to define camera
*/
/****************************************************************************/

#ifndef CAMERA_2_H
#define CAMERA_2_H

#include "Camera.h"

/****************************************************************************/
/*!
Class Camera2:
\brief Defines the methods for camera control
*/
/****************************************************************************/

class Camera2 : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera2();
	~Camera2();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
};

#endif