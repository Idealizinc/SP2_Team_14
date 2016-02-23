#ifndef _ROBOT_H
#define _ROBOT_H

#include "Vector3.h"
#include "Boundary.h"

class Robot
{
public:
<<<<<<< HEAD
	Robot();
	Robot(Vector3 Position, Vector3 DVector, float velocity) : RobotPos(Position), StoredDirVec(DVector), Speed(velocity){ };
=======
	enum R_Type
	{
		Melee = 0, Ranged, Mixed,
	};
	Robot(int RobotType, Vector3 SpawnPos, Boundary BBox = 0) : CurrPos(SpawnPos), BoundingBox(BBox){ SetStats(RobotType); }
>>>>>>> ace74c50eb2b882a60595127739e58615decd827
	~Robot();
	Vector3 Move();
	Vector3 Position();
	void BoundsCheck();
	float GetHealth();
	
private:
	Vector3 GetDirVec(Vector3 Target);
	void SetStats(int Type);
	R_Type RobotType;
	float Health;
	float Damage;
	float Speed;
	Vector3 TargetPos = (1, 1, 1);
	Vector3 DirVec;
	Vector3 CurrPos;
	Boundary BoundingBox;
};

#endif