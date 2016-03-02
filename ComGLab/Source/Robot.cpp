/****************************************************************************/
/*!
\file Robot.cpp
\author Ryan Lim Rui An, Teh Kai Hong Bryan
\par email: 150577L@mymail.nyp.edu.sg, 150587A@mymail.nyp.edu.sg
\brief
.cpp file that defines Robot
*/
/****************************************************************************/
#include "Robot.h"

#define PI 3.1415926535

/****************************************************************************/
/*!
\brief
Constructor
\param
		RobotType - value that determines the type of robot spawned
		Rotation - value that determines the angle which the robot faces
		SpawnPos - Vector3 value that determines the spawn point of a robot
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Destrutctor
*/
/****************************************************************************/

Robot::~Robot()
{

}

/****************************************************************************/
/*!
\brief
Method that handles each robot's movements
\param
		die - boolean which determines if the robot is dead or not
		CurrPos - Vector3 value which determines the robot's current position
		DirVec - Vector3 value which determines the robot's direction vector
		Speed - value which determines the robot's walking speed
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Chooses a new target for the robots to walk to
\param
		isMothership - boolean that determines if the current stage is the boss fight
		Randomize - value that is randomized
		Base - a boundary inside the base
		BaseOuter - a boundary around the base
		TargetPos - Vector3 value that determines the robots' target
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Sets a new target for the mothership
\param
	TargetPos - Vector3 value that determines the target
\return
Resulting normalized vectors
*/
/****************************************************************************/

void Robot::SetShipTarget(Vector3 newTarget)
{
	TargetPos = newTarget;
}

/****************************************************************************/
/*!
\brief
Calls the value for each robot's current position
\param
		CurrPos - Vector3 value which determines the robot's current position
\return
Resulting normalized vectors
*/
/****************************************************************************/

Vector3 Robot::Position()
{
	return CurrPos;
}

/****************************************************************************/
/*!
\brief
Checks if the player or a bullet has entered a robot's boundary
\param
		BulletUsed - boolean that checks if the bullet has hit a robot
		Health - value that determines each robot's health
		Damage - value that determines each bullet's damage
\return
Resulting values
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Calls the value for each robot's current heatlth
\param
		Health - value that determines each robot's health
\return
Resulting values
*/
/****************************************************************************/

float Robot::GetHealth()
{
	return Health;
}

/****************************************************************************/
/*!
\brief
Sets a new value for each robot's health
\param
	Health - value that determines each robot's health
\return
Resulting normalized vectors
*/
/****************************************************************************/

void Robot::SetHealth(float newHP)
{
	Health = newHP;
}

/****************************************************************************/
/*!
\brief
Animations for the robots
\param
		leftarmrotate - boolean that determines if the left arm is being rotated
		rotatelefthand - value that determines the angle of the left hand's rotation
		leftarmrotatelimite - value that determines the maximum angle of rotation for the left arm
		rightarmrotate - boolean that determines if the right arm is being rotated
		rotaterighthand - value that determines the angle of the right hand's rotation
		rightarmrotatelimite - value that determines the maximum angle of rotation for the right arm
		leftleg - boolean that determines if the left leg is moving
		moveleftleg - value that determines how much the left leg is moved
		rightleg - boolean that determines if the right leg is moving
		moverightleg - value that determines how much the right leg is moved
		die - boolean which determines if the robot is dead or not
		collapse - value for animating robot
		deadanimationover - boolean that determines if the death animation is over
\return
Resulting values
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Calculates the boundary of each robot
\param
		CurrPos - Vector3 value which determines the robot's current position
\return
Resulting normalized vectors
*/
/****************************************************************************/

void Robot::CalcBounds()
{
	if (RobotTypeVal != 3)
	{
		BoundingBox.set(CurrPos.x - 2.5, CurrPos.x + 2.5, CurrPos.z - 2.5, CurrPos.z + 2.5, CurrPos.y - 3, CurrPos.y + 7);
	}
	else 	BoundingBox.set(CurrPos.x - 30, CurrPos.x + 30, CurrPos.z - 60, CurrPos.z + 60, CurrPos.y - 20, CurrPos.y + 20);
}

/****************************************************************************/
/*!
\brief
Calculates the direction vector
\param
		DirVec - Vector3 value which determines the robot's direction vector
		Target - Vector3 value that determines the robots' target
		CurrPos - Vector3 value which determines the robot's current position
\return
Resulting normalized vectors
*/
/****************************************************************************/

Vector3 Robot::GetDirVec(Vector3 Target)
{
	DirVec = Target - CurrPos;
	if (DirVec.Length() > 0)
	{
		DirVec = DirVec.Normalize();
	}
	return DirVec;
}

/****************************************************************************/
/*!
\brief
Sets the values of each stat for every robot type
\param
		RobotType - value that determines the type of robot spawned
		Health - value that determines each robot's health
		Damage - value that determines each robot's damage
		Speed - value which determines the robot's walking speed
\return
Resulting values
*/
/****************************************************************************/

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
		Health = 120;
		Damage = 10;
		Speed = 0.1;
		break;
	case 2:
		RobotType = Melee;
		Health = 140;
		Damage = 10;
		Speed = 0.1;
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