#ifndef _WEAPONSYSTEM_H
#define _WEAPONSYSTEM_H

#include <vector>
#include "RayCast.h"

class WeaponSystem
{
public:
	WeaponSystem(){};
	~WeaponSystem();
	static unsigned short BulletCount;
	void RenderBulletList();
	std::vector<RayCast> BulletList;

};

#endif