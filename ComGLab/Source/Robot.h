#ifndef _ROBOT_H
#define _ROBOT_H
#include "Vector3.h"

class Robot
{
	
public:
	Robot();
	Robot(Vector3 Position, Vector3 DVector, float velocity) : RobotPos(Position), StoredDirVec(DVector), Speed(velocity){ };
	~Robot();
	Vector3 Move();
	Vector3 Position();
	enum ROBOTATTRIBUTES
	{
		MeleeDmg = 5,
		RangeDmg = 5,
		MixedMeleeDmg = 4,
		MixedRangeDmg = 4,
		RobotHP = 20,
	};
private:
	float Speed;
	Vector3 StoredDirVec;
	Vector3 RobotPos;
	float increment = 0;
};


#endif