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
	void FindNewTarget();
	Vector3 Position();
	void BoundsCheck(WeaponSystem WepSys);
	float GetHealth();
	Boundary BoundingBox;
	void SetHealth(float newHP);
	void findAngle();
	float rotateToTarget;
	void RobotAnimation(double dt);
	void CalcBounds();

	float rotatelefthand = 0;
	float rotaterighthand = 0;
	float leftarmrotatelimit = -1;
	float rightarmrotatelimit = -1;
	bool leftarmrotate = true;
	bool rightarmrotate = true;

	float leftleglimit = 4;
	float rightleglimit = 4;
	float moveleftleg = 0;
	float moverightleg = 0;
	bool leftleg = true;
	bool rightleg = true;

	bool robotleftattack;
	bool robotrightattack;
	float rightarmattack = 0;
	float rightarmattacklimit = -4;
	float leftarmattack = 0;
	float leftarmattacklimit = -4;

	float collapse = 0;
	bool die = false;
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