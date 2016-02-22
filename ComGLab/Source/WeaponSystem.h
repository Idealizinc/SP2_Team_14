#ifndef _WEAPONSYSTEM_H
#define _WEAPONSYSTEM_H

#include <list>
#include "RayCast.h"

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

private:
	unsigned short MaxBulletCount = 500;
	unsigned short MaxDist = 10000;
};

#endif