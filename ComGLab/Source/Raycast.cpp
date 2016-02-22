#include "RayCast.h"

RayCast::~RayCast()
{

}

Vector3 RayCast::Move()
{
	//CalcIncrement(val);
	RayPos.x += StoredDirVec.x * Speed;
	RayPos.y += StoredDirVec.y * Speed;
	RayPos.z += StoredDirVec.z * Speed;
	return RayPos;
}

Vector3 RayCast::Position()
{
	return RayPos;
}

void RayCast::BoundsCheck()
{
	// If Bullet in specific bounds, Wall Collision
	// Pop Bullet. 
	// If Bullet in Robot Bounds, Damage Robot
	// Pop Bullet
}