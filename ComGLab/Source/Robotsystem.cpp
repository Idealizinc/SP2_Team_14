#include "Robotsystem.h"

Robotsystem::~Robotsystem()
{
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		iter = RobotList.erase(iter);
	}
}

void Robotsystem::IncrementPosition()
{
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); ++iter)
	{
		(*iter).Move();
	}
}

void Robotsystem::CleanUp()
{
	if (RobotList.size() > MaxRobotCount)
	{
		RobotList.pop_front();
	}
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		if (GetLength(*iter) > MaxDist)
		{
			iter = RobotList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Robotsystem::ClearList()
{
	for (std::list<Robot>::iterator iter = RobotList.begin(); iter != RobotList.end(); /*++iter*/)
	{
		iter = RobotList.erase(iter);
	}
}

float Robotsystem::GetLength(Robot robot)
{
	return robot.Position().Length();
}