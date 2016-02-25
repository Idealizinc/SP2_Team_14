#include "Robot.h"

#define PI 3.1415926535

Robot::Robot(int RobotType, Vector3 SpawnPos)
{
	CurrPos = SpawnPos;
	GetDirVec(TargetPos);
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

	//float dot, lenSq1, lenSq2;

	//dot = (0 * DirVec.x) + (0 * DirVec.y) + (1 * DirVec.z);
	//lenSq1 = 0 + 0 + 1;
	//lenSq2 = (DirVec.x * DirVec.x) + (DirVec.y * DirVec.y) + (DirVec.z * DirVec.z);
	//rotateToTarget = acos(dot / sqrt(lenSq1 * lenSq2));

	/*std::cout << DirVec.x << ", " << DirVec.y << ", " << DirVec.z << std::endl;
	std::cout << rotateToTarget << std::endl;
	*/float dot, det;

dot = (0 * DirVec.x) + (1 * DirVec.z);
det = (0 * DirVec.z) - (1 * DirVec.x);
rotateToTarget = atan2(det, dot);
//std::cout << DirVec.x << ", " << DirVec.y << ", " << DirVec.z << std::endl;
//std::cout << rotateToTarget << std::endl;
}

Vector3 Robot::Move()
{
	GetDirVec(TargetPos);
	CurrPos.x += DirVec.x * Speed;
	CurrPos.y += DirVec.y * Speed;
	CurrPos.z += DirVec.z * Speed;
	CalcBounds();
	return CurrPos;
}

Vector3 Robot::Position()
{
	return CurrPos;
}

void Robot::BoundsCheck(WeaponSystem WepSys)
{
	//Bot Bullet Trigger Check
	for (auto i : WepSys.BulletList)
	{
		if (!BoundingBox.BoundaryCheck(i.Position().x, i.Position().z, i.Position().y))
		{
			//Health = Health - i.Damage;
			// Weapon Sys Detects if True, if so pop shot.
			i.BulletUsed = true;
			SetHealth(Health - i.Damage);
		}
	}
	//for (std::list<RayCast>::iterator iter = WepSys.BulletList.begin(); iter != WepSys.BulletList.end(); /*++iter*/)
	//{
	//	if (!BoundingBox.BoundaryCheck((*iter).Position().x, (*iter).Position().z, (*iter).Position().y))
	//	{
	//		// Weapon Sys Detects if True, if so pop shot.
	//		SetHealth(Health - (*iter).Damage);
	//		(*iter).BulletUsed = true;
	//	}
	//	else ++iter;
	//}

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
	BoundingBox.set(CurrPos.x - 3, CurrPos.x + 3, CurrPos.z - 3, CurrPos.z + 3, CurrPos.y - 3, CurrPos.y + 7);
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
		Speed = 0.1;
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