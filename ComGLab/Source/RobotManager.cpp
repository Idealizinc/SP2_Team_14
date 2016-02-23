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
		RobotList.pop_front();
	}
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		/*if (GetDist(*iter) > MaxDist)
		{
			iter = RobotList.erase(iter);
		}*/
		if ((*iter).GetHealth() <= 0)
		{
			iter = RobotList.erase(iter);
		}
		else
		{
			++iter;
		}
	}

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