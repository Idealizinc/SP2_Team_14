#ifndef _ROBOT_H
#define _ROBOT_H

#include "Vector3.h"
#include "Boundary.h"
#include <math.h>
#include <cmath>

class Robot
{
public:
	Robot();
	enum R_Type
	{
		Melee = 0, Ranged, Mixed,
	};
	Robot(int RobotType, Vector3 SpawnPos);
	~Robot();
	Vector3 Move();
	Vector3 Position();
	void BoundsCheck();
	float GetHealth();
	Boundary BoundingBox;
	void SetHealth(float newHP);
	void findAngle();
	float rotateToTarget;
	void CalcBounds();
	float Health;

private:
	Vector3 GetDirVec(Vector3 Target);
	void SetStats(int Type);
	R_Type RobotType;
	float Damage;
	float Speed;
	Vector3 TargetPos = (1, 1, 1);
	Vector3 DirVec;
	Vector3 CurrPos;
};

#endif