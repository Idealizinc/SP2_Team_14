#include "Robot.h"

#define PI 3.1415926535

Robot::Robot(int RobotType, Vector3 SpawnPos) : CurrPos(SpawnPos)
{ 
	findAngle();
	SetStats(RobotType); 
	CalcBounds();
	//(CurrPos.x + 3) * rotateToTarget
}

Robot::~Robot()
{

}

void Robot::findAngle()
{
	/*float A, B, C, total;
	A = ((0 * DirVec.x) + (0 * DirVec.y) + (1 * DirVec.z));

	B = pow(0, 2) + pow(0, 2), pow(1, 2);
	B = sqrt(B);

	B = pow(0, 2) + pow(1, 2), pow(0, 2);
	B = sqrt(B);

	C = pow(DirVec.x, 2) + pow(DirVec.z, 2) + pow(DirVec.y, 2);
	C = sqrt(C);

	total = (B * C);

	rotateToTarget = A / total;

	rotateToTarget = acos(rotateToTarget) * 180.0 / PI;*/

	/*float A;

	A = atan2(1 - DirVec.z, 0 - DirVec.x);
	rotateToTarget = A * 180.0 / PI;*/

	float dot, lenSq1, lenSq2;

	dot = (0 * DirVec.x) + (0 * DirVec.y) + (1 * DirVec.z);
	lenSq1 = 0 + 0 + 1;
	lenSq2 = (DirVec.x * DirVec.x) + (DirVec.y * DirVec.y) + (DirVec.z * DirVec.z);
	rotateToTarget = acos(dot / sqrt(lenSq1 * lenSq2));
}

Vector3 Robot::Move()
{
	GetDirVec(TargetPos);
	//CalcIncrement(val);
	CurrPos.x += DirVec.x * Speed;
	CurrPos.y += DirVec.y * Speed;
	CurrPos.z += DirVec.z * Speed;
	CalcBounds();
	/*BoundingBox.xMax += DirVec.x * Speed;
	BoundingBox.xMin += DirVec.x * Speed;
	BoundingBox.yMax += DirVec.y * Speed;
	BoundingBox.yMin += DirVec.y * Speed;
	BoundingBox.zMax += DirVec.z * Speed;
	BoundingBox.zMin += DirVec.z * Speed;*/
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

void Robot::SetHealth(float newHP)
{
	Health = newHP;
}

void Robot::CalcBounds()
{
	BoundingBox.set(CurrPos.x - 3, CurrPos.x + 3, CurrPos.z - 3, CurrPos.z + 3, CurrPos.y - 3, CurrPos.y + 3);
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
		Speed = 1;
		break;
	case 1:
		RobotType = Ranged;
		Health = 30;
		Damage = 5;
		Speed = 1;
		break;
	case 2:
		RobotType = Mixed;
		Health = 40;
		Damage = 8;
		Speed = 1;
		break;
	default:
		Health = 0;
		Damage = 0;
		Speed = 0;
		break;
	}
}