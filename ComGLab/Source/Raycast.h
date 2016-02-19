#ifndef _RAYCAST_H
#define _RAYCAST_H

#include "Vector3.h"
#include "Boundary.h"

class RayCast
{
public:
	RayCast(Vector3 Position, Vector3 DVector, float velocity, Boundary BBox = 0) : RayPos(Position), StoredDirVec(DVector), Speed(velocity), BoundingBox(BBox){ };
	~RayCast();
	void Move();
	Vector3 Position();
	void RenderRay();

private:
	float Speed;
	Vector3 StoredDirVec;
	Vector3 RayPos;
	Boundary BoundingBox;
};

#endif