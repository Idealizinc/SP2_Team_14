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

};

#endif