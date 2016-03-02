/****************************************************************************/
/*!
\file Raycast.h
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
Class to define RayCast
*/
/****************************************************************************/
#ifndef _RAYCAST_H
#define _RAYCAST_H

#include "Vector3.h"
#include "Boundary.h"

/****************************************************************************/
/*!
Class RayCast:
\brief Defines the methods and variables for RayCasting
*/
/****************************************************************************/

class RayCast
{
public:
	RayCast(int Damage, float velocity, Vector3 Position, Vector3 DVector) : RayPos(Position), StoredDirVec(DVector), Damage(Damage), Speed(velocity){ CalcBounds(); };
	~RayCast();
	Vector3 Move();
	Vector3 Position();
	//bool BoundsCheck(std::list<Robot> RobotList);
	void CalcBounds();
	float Damage = 0;
	bool BulletUsed = false;

private:
	float Speed = 0;
	Vector3 StoredDirVec;
	Vector3 RayPos;
	Boundary BoundingBox;
	float increment = 0;
};

#endif