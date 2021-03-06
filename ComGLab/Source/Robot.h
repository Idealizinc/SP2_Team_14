/****************************************************************************/
/*!
\file Robot.h
\author Ryan Lim Rui An, Teh Kai Hong Bryan
\par email: 150577L@mymail.nyp.edu.sg, 150587A@mymail.nyp.edu.sg
\brief
Class to define Robot
*/
/****************************************************************************/

#ifndef _ROBOT_H
#define _ROBOT_H

#include "Vector3.h"
#include "WeaponSystem.h"
#include "Boundary.h"
#include <math.h>
#include <cmath>

/****************************************************************************/
/*!
Class Robot:
\brief Defines the methods, enums and variables for Robot
*/
/****************************************************************************/

class Robot
{
public:
	Robot();
	enum R_Type
	{
		Melee = 0, Ranged, Mixed,
	};
	Robot(int RobotType, float Rotation, Vector3 SpawnPos);
	~Robot();
	Vector3 Move();
	void FindNewTarget();
	Vector3 Position();
	void BoundsCheck(WeaponSystem WepSys);
	float GetHealth();
	Boundary BoundingBox;
	void SetHealth(float newHP);
	float rotateToTarget;
	void RobotAnimation(double dt);
	void CalcBounds();
	bool isMothership = false;
	void SetShipTarget(Vector3 newTarget);

	//Animation
	float Rotation;
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
	bool deadanimationover = false;
	float explosion = 0;

private:
	Vector3 GetDirVec(Vector3 Target);
	void SetStats(int Type);
	R_Type RobotType;
	float Damage;
	float Speed;
	float Health;
	int RobotTypeVal = 0;
	

	Vector3 TargetPos = (0, 0, 0);
	Vector3 DirVec;
	Vector3 CurrPos;
};

#endif