/****************************************************************************/
/*!
\file WeaponSystem.cpp
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
.cpp file that defines Weapon System
*/
/****************************************************************************/

#include "WeaponSystem.h"


/****************************************************************************/
/*!
\brief
Constructor
*/
/****************************************************************************/

WeaponSystem::~WeaponSystem()
{
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); /*++iter*/)
	{
		iter = BulletList.erase(iter);
	}
}


/****************************************************************************/
/*!
\brief
Increments the position of each bullet
*/
/****************************************************************************/

void WeaponSystem::IncrementPosition()
{
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); ++iter)
	{
		(*iter).Move();
	}
}


/****************************************************************************/
/*!
\brief
Cleans up data to prevent memory leaks
*/
/****************************************************************************/

void WeaponSystem::CleanUp()
{
	if (BulletList.size() > MaxBulletCount)
	{
		BulletList.pop_front();
	}
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); /*++iter*/)
	{
		if ((*iter).BulletUsed == true)
		{
			iter = BulletList.erase(iter);
		}
		else if (GetLength(*iter) > MaxDist)
		{
			iter = BulletList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}


/****************************************************************************/
/*!
\brief
Removes bullets from the list
*/
/****************************************************************************/

void WeaponSystem::ClearList()
{
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); /*++iter*/)
	{
		iter = BulletList.erase(iter);
	}
}


/****************************************************************************/
/*!
\brief
Returns the length of each bullet
*/
/****************************************************************************/

float WeaponSystem::GetLength(RayCast Ray)
{
	return Ray.Position().Length();
}


/****************************************************************************/
/*!
\brief
Sets the stats for each type of gun
\param
		wepVal - value that determines the type of gun used
		ReloadTime - value that determines how long each gun takes to reload
		RateOfFire - value that determines each weapon's rate of fire
		Damage - value that determines each weapon's damage
		Speed - value that determines the speed of each bullet
		MaxAmmo - value that determines the maximum amount of ammo that each type of gun can hold
\return
Resulting values
*/
/****************************************************************************/

void WeaponSystem::SetStats(int wepVal)
{
	switch (wepVal)
	{
	case 0:
		ReloadTime = 3;
		RateOfFire = 0.1;
		Damage = 5;
		Speed = 4;
		MaxAmmo = 30;
		break;
	case 1:
		ReloadTime = 2;
		RateOfFire = 0.15;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 40;
		break;
	case 2:
		ReloadTime = 3;
		RateOfFire = 0.5;
		Damage = 70;
		Speed = 5;
		MaxAmmo = 10;
		break;
	case 3:
		ReloadTime = 2;
		RateOfFire = 0.1;
		Damage = 5;
		Speed = 4;
		MaxAmmo = 30;
		break;
	case 4:
		ReloadTime = 2;
		RateOfFire = 0.15;
		Damage = 12;
		Speed = 4;
		MaxAmmo = 40;
		break;
	case 5:
		ReloadTime = 3;
		RateOfFire = 0.5;
		Damage = 140;
		Speed = 5;
		MaxAmmo = 10;
		break;
	case 6:
		ReloadTime = 2;
		RateOfFire = 0.1;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 30;
		break;
	case 7:
		ReloadTime = 2;
		RateOfFire = 0.15;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 80;
		break;
	case 8:
		ReloadTime = 3;
		RateOfFire = 0.5;
		Damage = 70;
		Speed = 5;
		MaxAmmo = 20;
		break;
	case 9:
		ReloadTime = 2;
		RateOfFire = 0.1;
		Damage = 5;
		Speed = 4;
		MaxAmmo = 60;
		break;
	case 10:
		ReloadTime = 1;
		RateOfFire = 0.075;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 40;
		break;
	case 11:
		ReloadTime = 2;
		RateOfFire = 0.3;
		Damage = 70;
		Speed = 5;
		MaxAmmo = 10;
		break;
	case 12:
		ReloadTime = 1;
		RateOfFire = 0.05;
		Damage = 5;
		Speed = 4;
		MaxAmmo = 30;
		break;
	case 13:
		ReloadTime = 0;
		RateOfFire = 0;
		Damage = 0;
		Speed = 0;
		MaxAmmo = 0;
		break;
	case 14:
		ReloadTime = 3;
		RateOfFire = 0.15;
		Damage = 3;
		Speed = 15;
		MaxAmmo = 50;
		break;
	default:
		ReloadTime = 0;
		RateOfFire = 0;
		Damage = 0;
		Speed = 0;
		MaxAmmo = 0;
		break;
	}
}