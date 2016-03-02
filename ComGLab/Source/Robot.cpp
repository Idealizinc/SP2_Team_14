#include "Robot.h"

#define PI 3.1415926535

Robot::Robot(int RobotType, float Rotation, Vector3 SpawnPos)
{
	this->RobotTypeVal = RobotType;
	CurrPos = SpawnPos;
	GetDirVec(TargetPos);
	SetStats(RobotType);
	CalcBounds();
	this->Rotation = Rotation;
	//RobotAnimation(dt);
	//(CurrPos.x + 3) * rotateToTarget
}

Robot::~Robot()
{

}


Vector3 Robot::Move()
{
	if (GetHealth() <= 0)
	{
		die = true;
	}
	else
	{
		FindNewTarget();
		GetDirVec(TargetPos);
		CurrPos.x += DirVec.x * Speed;
		CurrPos.y += DirVec.y * Speed;
		CurrPos.z += DirVec.z * Speed;
		CalcBounds();
		return CurrPos;
	}
}

void Robot::FindNewTarget()
{
	if (!isMothership)
	{
		int Randomize = rand() % 10 + 1;
		if (Randomize > 5)
		{
			Boundary Base(-19, 19, -19, 19, -5, 10);
			Boundary BaseOuter(-25, 25, -25, 25, -5, 10);
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
}

void Robot::SetShipTarget(Vector3 newTarget)
{
	TargetPos = newTarget;
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
			i.BulletUsed = true;
			SetHealth(Health - i.Damage);
		}
	}
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
		moveleftleg -= (float)(20 * dt);
	}
	else if (leftleg == false)
	{
		moveleftleg += (float)(20 * dt);
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
		moverightleg += (float)(20 * dt);
	}
	else if (rightleg == false)
	{
		moverightleg -= (float)(20 * dt);
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

	

	if (die == true)
	{
		rotatelefthand = 0;
		rotaterighthand = 0;
		moveleftleg = 0;
		moverightleg = 0;
		collapse += (float)(100 * dt);
		if (collapse > 85)
		{
			die = false;
			collapse -= (float)(100 * dt);
			deadanimationover = true;
		}
	}

	//droid repair animation
	
}

void Robot::CalcBounds()
{
	if (RobotTypeVal != 3)
	{
		BoundingBox.set(CurrPos.x - 2.5, CurrPos.x + 2.5, CurrPos.z - 2.5, CurrPos.z + 2.5, CurrPos.y - 3, CurrPos.y + 7);
	}
	else 	BoundingBox.set(CurrPos.x - 30, CurrPos.x + 30, CurrPos.z - 60, CurrPos.z + 60, CurrPos.y - 20, CurrPos.y + 20);
}

Vector3 Robot::GetDirVec(Vector3 Target)
{
	DirVec = Target - CurrPos;
	if (DirVec.Length() > 0)
	{
		DirVec = DirVec.Normalize();
	}
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
		Speed = 0.15;
		break;
	case 1:
		RobotType = Melee;
		Health = 120;
		Damage = 10;
		Speed = 0.15;
		break;
	case 2:
		RobotType = Melee;
		Health = 140;
		Damage = 10;
		Speed = 0.175;
		break;
	case 3:
		//RobotType = ; Mothership
		Health = 5000;
		Damage = 10;
		Speed = 0.2;
		break;
	default:
		Health = 0;
		Damage = 0;
		Speed = 0;
		break;
	}
}