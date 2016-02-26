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

void WeaponSystem::SetStats(int wepVal)
{
	switch (wepVal)
	{
	case 0:
		ReloadTime = 3;
		RateOfFire = 0;// 0.1;
		Damage = 5;
		Speed = 3;
		MaxAmmo = 1337;// 30;
		break;
	case 1:
		ReloadTime = 4;
		RateOfFire = 0.15;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 40;
		break;
	case 2:
		ReloadTime = 5;
		RateOfFire = 0.5;
		Damage = 70;
		Speed = 5;
		MaxAmmo = 10;
		break;
	case 3:
		ReloadTime = 3;
		RateOfFire = 0.1;
		Damage = 5;
		Speed = 3;
		MaxAmmo = 30;
		break;
	case 4:
		ReloadTime = 4;
		RateOfFire = 0.15;
		Damage = 12;
		Speed = 4;
		MaxAmmo = 40;
		break;
	case 5:
		ReloadTime = 5;
		RateOfFire = 0.5;
		Damage = 120;
		Speed = 5;
		MaxAmmo = 10;
		break;
	case 6:
		ReloadTime = 3;
		RateOfFire = 0.1;
		Damage = 8;
		Speed = 3;
		MaxAmmo = 30;
		break;
	case 7:
		ReloadTime = 4;
		RateOfFire = 0.15;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 60;
		break;
	case 8:
		ReloadTime = 5;
		RateOfFire = 0.5;
		Damage = 70;
		Speed = 5;
		MaxAmmo = 15;
		break;
	case 9:
		ReloadTime = 3;
		RateOfFire = 0.1;
		Damage = 5;
		Speed = 3;
		MaxAmmo = 45;
		break;
	case 10:
		ReloadTime = 2;
		RateOfFire = 0.075;
		Damage = 8;
		Speed = 4;
		MaxAmmo = 40;
		break;
	case 11:
		ReloadTime = 3;
		RateOfFire = 0.3;
		Damage = 70;
		Speed = 5;
		MaxAmmo = 10;
		break;
	case 12:
		ReloadTime = 1;
		RateOfFire = 0.05;
		Damage = 5;
		Speed = 3;
		MaxAmmo = 30;
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