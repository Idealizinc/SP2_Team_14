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

bool RayCast::BoundsCheck(std::list<Robot> RobotList)
{
	// If Bullet in specific bounds, Wall Collision
	// Pop Bullet. 

	//Bot Bullet Trigger Check
	for (auto i : RobotList)
	{
		if (!i.BoundingBox.BoundaryCheck(RayPos.x, RayPos.z, RayPos.y))
		{
			i.SetHealth(i.GetHealth() - Damage);
			return true;
			// Weapon Sys Detects if True, if so pop shot.
		}
	}
	return false;
}