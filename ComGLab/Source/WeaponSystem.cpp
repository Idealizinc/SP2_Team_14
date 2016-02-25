#include "WeaponSystem.h"

WeaponSystem::~WeaponSystem()
{
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); /*++iter*/)
	{
		iter = BulletList.erase(iter);
	}
}

void WeaponSystem::IncrementPosition()
{
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); ++iter)
	{
		(*iter).Move();
	}
}

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

void WeaponSystem::ClearList()
{
	for (std::list<RayCast>::iterator iter = BulletList.begin(); iter != BulletList.end(); /*++iter*/)
	{
		iter = BulletList.erase(iter);
	}
}

float WeaponSystem::GetLength(RayCast Ray)
{
	return Ray.Position().Length();
}