#include "Robot.h"

Robot::~Robot()
{

}

Vector3 Robot::Move()
{
	GetDirVec(TargetPos);
	//CalcIncrement(val);
	CurrPos.x += DirVec.x * Speed;
	CurrPos.y += DirVec.y * Speed;
	CurrPos.z += DirVec.z * Speed;
	//BoundingBox.xMax += StoredDirVec.x * Speed;
	return CurrPos;
}

Vector3 Robot::Position()
{
	return CurrPos;
}

void Robot::BoundsCheck()
{
	// If Bullet in specific bounds, Wall Collision
	// Pop Bullet. 
	// If Bullet in Robot Bounds, Damage Robot
	// Pop Bullet
}

float Robot::GetHealth()
{
	return Health;
}

Vector3 Robot::GetDirVec(Vector3 Target)
{
	DirVec = Target - CurrPos;
	DirVec = DirVec.Normalize();
	return DirVec;
}

void Robot::SetStats(int Type)
{
	switch (Type)
	{
	case 0:
		RobotType = Melee;
		Health = 50;
		Damage = 10;
		Speed = 2;
		break;
	case 1:
		RobotType = Ranged;
		Health = 30;
		Damage = 5;
		Speed = 2;
		break;
	case 2:
		RobotType = Mixed;
		Health = 40;
		Damage = 8;
		Speed = 2;
		break;
	default:
		Health = 0;
		Damage = 0;
		Speed = 0;
		break;
	}
}