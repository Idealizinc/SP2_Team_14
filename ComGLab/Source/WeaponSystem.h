/****************************************************************************/
/*!
\file WeaponSystem.h
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
Header file for Weapon System
*/
/****************************************************************************/
#ifndef _WEAPONSYSTEM_H
#define _WEAPONSYSTEM_H

#include <list>
#include "RayCast.h"

/****************************************************************************/
/*!
Class WeaponSystem:
\brief Defines the methods and variables for Weapon System
*/
/****************************************************************************/

class WeaponSystem
{
public:
	WeaponSystem(){};
	~WeaponSystem();
	std::list<RayCast> BulletList;
	void IncrementPosition();
	void CleanUp();
	float GetLength(RayCast Ray);
	void ClearList();
	void SetStats(int wepVal);

	float ReloadTime;
	float RateOfFire;
	float Damage;
	float Speed;
	float MaxAmmo;

private:
	unsigned short MaxBulletCount = 100;
	unsigned short MaxDist = 10000;
};

#endif