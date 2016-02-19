#include "WeaponSystem.h"

WeaponSystem::~WeaponSystem()
{
	for (std::vector<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); /*++iter*/)
	{
		iter = BulletList.erase(iter);
	}
}