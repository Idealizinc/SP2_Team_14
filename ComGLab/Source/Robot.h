#ifndef _ROBOT_H
#define _ROBOT_H

#include "Vector3.h"
#include "WeaponSystem.h"
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
	Vector3 FindNewTarget();
	Vector3 Position();
	void BoundsCheck(WeaponSystem WepSys);
	float GetHealth();
	Boundary BoundingBox;
	void SetHealth(float newHP);
	void findAngle();
	float rotateToTarget;
	void RobotAnimation(double dt);
	void CalcBounds();
private:
	Vector3 GetDirVec(Vector3 Target);
	void SetStats(int Type);
	R_Type RobotType;
	float Damage;
	float Speed;
	float Health;
	Vector3 TargetPos = (0, 0, 0);
	Vector3 DirVec;
	Vector3 CurrPos;
};

#endif