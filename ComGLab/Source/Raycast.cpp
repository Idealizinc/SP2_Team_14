#include "RayCast.h"

RayCast::~RayCast()
{

}

void RayCast::CalcIncrement(float val)
{
	increment += Speed * val;
}

Vector3 RayCast::Move(float val)
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