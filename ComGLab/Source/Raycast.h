#ifndef _RAYCAST_H
#define _RAYCAST_H

#include "Vector3.h"
#include "Boundary.h"
#include "RobotManager.h"

class RayCast
{
public:
	RayCast(Vector3 Position, Vector3 DVector, float velocity) : RayPos(Position), StoredDirVec(DVector), Speed(velocity){ CalcBounds(); };
	~RayCast();
	Vector3 Move();
	Vector3 Position();
	bool BoundsCheck(std::list<Robot> RobotList);
	void CalcBounds();

private:
	float Damage = 420;
	float Speed;
	Vector3 StoredDirVec;
	Vector3 RayPos;
	Boundary BoundingBox;
	float increment = 0;
};

#endif