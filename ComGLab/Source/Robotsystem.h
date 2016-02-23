#ifndef _ROBOTSYSTEM_H
#define _ROBOTSYSTEM_H

#include <list>
#include "Robot.h"

class Robotsystem
{
public:
	Robotsystem(){};
	~Robotsystem();
	std::list<Robot> RobotList;
	void IncrementPosition();
	void CleanUp();
	float GetLength(Robot robot);
	void ClearList();
	unsigned short MaxRobotCount = 20;
private:
	unsigned short MaxDist = 10;
};

#endif