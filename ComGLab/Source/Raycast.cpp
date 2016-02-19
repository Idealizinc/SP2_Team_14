#include "RayCast.h"

RayCast::~RayCast()
{

}

void RayCast::Move(float dt)
{
	RayPos += StoredDirVec * dt * Speed;
}

Vector3 RayCast::Position()
{
	return RayPos;
}