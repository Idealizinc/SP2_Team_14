#ifndef _ROBOTMANAGER_H
#define _ROBOTMANAGER_H

#include <list>
#include "Robot.h"

class RobotManager
{
public:
	RobotManager(){};
	~RobotManager();
	std::list<Robot> RobotList;
	void IncrementPosition();
	float GetDist(Robot Bot);
	void ClearList();
	void CleanUp();
	unsigned short CurrentRobotCount = 0;

private:
	unsigned short MaxRobotCount = 50;
	//unsigned short MaxDist = 10000;
};

#endif