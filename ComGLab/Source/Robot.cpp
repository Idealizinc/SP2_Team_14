#include "Robot.h"

#define PI 3.1415926535

Robot::Robot(int RobotType, Vector3 SpawnPos)
{
	CurrPos = SpawnPos;
	GetDirVec(TargetPos);
	findAngle();
	SetStats(RobotType);
	CalcBounds();
	//RobotAnimation(dt);
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
	*/
	float dot, det;
	dot = (0 * DirVec.x) + (1 * DirVec.z);
	det = (0 * DirVec.z) - (1 * DirVec.x);
	rotateToTarget = Math::RadianToDegree(atan2(det, dot));
	//std::cout << DirVec.x << ", " << DirVec.y << ", " << DirVec.z << std::endl;
	//std::cout << rotateToTarget << std::endl;

	/*Vector3 a(0, 0, 1);
	float dot2 = DirVec.Dot(a);
	rotateToTarget = Math::RadianToDegree(acos(dot2 / sqrt(a.LengthSquared() * DirVec.LengthSquared())));
	std::cout << rotateToTarget << std::endl;*/
}

Vector3 Robot::Move()
{
	FindNewTarget();
	GetDirVec(TargetPos);
	CurrPos.x += DirVec.x * Speed;
	CurrPos.y += DirVec.y * Speed;
	CurrPos.z += DirVec.z * Speed;
	CalcBounds();

	return CurrPos;
}

void Robot::FindNewTarget()
{
	int Randomize = rand() % 10 + 1;
	if (Randomize > 5)
	{
		Boundary Base(-19, 19, -19, 19, -5, 10);
		Boundary BaseOuter(-20, 20, -20, 20, -5, 10);
		if (BaseOuter.BoundaryCheck(CurrPos.x, CurrPos.y, CurrPos.z))
		{
			int choice = rand() % 4 + 1;
			switch (choice)
			{
			case 1: TargetPos = (20, 0, 0);
				break;
			case 2: TargetPos = (-20, 0, 0);
				break;
			case 3: TargetPos = (0, 0, 20);
				break;
			case 4: TargetPos = (0, 0, -20);
				break;
			default: TargetPos = (0, 0, 0);
				break;
			}
		}
		else TargetPos = (0, 0, 0);
	}
	else TargetPos = (0, 0, 0);
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
void Robot::RobotAnimation(double dt)
{
	//left arm
	if (leftarmrotate == true)
	{
		rotatelefthand -= (float)(5 * dt);
	}
	else if (leftarmrotate == false)
	{
		rotatelefthand += (float)(5 * dt);
	}
	if (rotatelefthand <= leftarmrotatelimit)
	{
		leftarmrotate = false;
	}
	else if (rotatelefthand >= -leftarmrotatelimit)
	{
		leftarmrotate = true;
	}

	//right arm
	if (rightarmrotate == true)
	{
		rotaterighthand += (float)(5 * dt);
	}
	else if (rightarmrotate == false)
	{
		rotaterighthand -= (float)(5 * dt);
	}
	if (rotaterighthand <= rightarmrotatelimit)
	{
		rightarmrotate = true;
	}
	else if (rotaterighthand >= -rightarmrotatelimit)
	{
		rightarmrotate = false;
	}
	//left leg
	if (leftleg == true)
	{
		moveleftleg -= (float)(10 * dt);
	}
	else if (leftleg == false)
	{
		moveleftleg += (float)(10 * dt);
	}
	if (moveleftleg >= leftleglimit)
	{
		leftleg = true;
	}
	else if (moveleftleg <= -leftleglimit)
	{
		leftleg = false;
	}

	//right leg
	if (rightleg == true)
	{
		moverightleg += (float)(10 * dt);
	}
	else if (rightleg == false)
	{
		moverightleg -= (float)(10 * dt);
	}
	if (moverightleg >= rightleglimit)
	{
		rightleg = false;
	}
	else if (moverightleg <= -rightleglimit)
	{
		rightleg = true;
	}
	//walking
	/*if (walk == false)
	{
		leftarmrotatelimit = -3;
		rightarmrotatelimit = -3;
	}*/

	//if (Robot::RobotHP == 0)
	//{
	//	die = true;
	//	curRobotCount--;
	//}

	if (die == true)
	{
		collapse += (float)(15 * dt);
		if (collapse > 85)
		{
			die = false;
			collapse -= (float)(15 * dt);
		}
	}

	//droid repair animation
	
}
void Robot::CalcBounds()
{
	BoundingBox.set(CurrPos.x - 2.5, CurrPos.x + 2.5, CurrPos.z - 2.5, CurrPos.z + 2.5, CurrPos.y - 3, CurrPos.y + 7);
}

Vector3 Robot::GetDirVec(Vector3 Target)
{
	DirVec = Target - CurrPos;
	DirVec = DirVec.Normalize();
	return DirVec;
}

void Robot::SetStats(int Level)
{
	switch (Level)
	{
	case 0:
		RobotType = Melee;
		Health = 100;
		Damage = 10;
		Speed = 0.1;
		break;
	case 1:
		RobotType = Melee;
		Health = 150;
		Damage = 10;
		Speed = 0.1;
		break;
	case 2:
		RobotType = Melee;
		Health = 200;
		Damage = 10;
		Speed = 0.1;
		break;
	default:
		Health = 0;
		Damage = 0;
		Speed = 0;
		break;
	}
}