#include "RayCast.h"

RayCast::~RayCast()
{

}

void RayCast::Move()
{
	RayPos.x = RayPos.x + StoredDirVec.x * Speed;
	RayPos.y = RayPos.y + StoredDirVec.y * Speed;
	RayPos.z = RayPos.z + StoredDirVec.z * Speed;

}

Vector3 RayCast::Position()
{
	return RayPos;
}