/****************************************************************************/
/*!
\file RobotManager.cpp
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
.cpp file that manages robots in the scene
*/
/****************************************************************************/
#include "RobotManager.h"

RobotManager::~RobotManager()
{
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		iter = RobotList.erase(iter);
	}
}

void RobotManager::IncrementPosition()
{
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); ++iter)
	{
		(*iter).Move();
	}
}

void RobotManager::CleanUp()
{
	if (RobotList.size() > MaxRobotCount)
	{
		RobotList.pop_back();
	}
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		/*if (GetDist(*iter) > MaxDist)
		{
		iter = RobotList.erase(iter);
		}*/
		if ((*iter).GetHealth() <= 0 && (*iter).deadanimationover)
		{
			iter = RobotList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	CurrentRobotCount = RobotList.size();
}

void RobotManager::ClearList()
{
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		iter = RobotList.erase(iter);
	}
}

//float RobotManager::GetDist(Robot Ray)
//{
//	return Ray.Position().Length();
//}