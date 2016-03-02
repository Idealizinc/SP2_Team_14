/****************************************************************************/
/*!
\file RobotManager.h
\author Ryan Lim Rui An
\par email: 150577@mymail.nyp.edu.sg
\brief
Class that defines the system that manages the robots
*/
/****************************************************************************/
#ifndef _ROBOTMANAGER_H
#define _ROBOTMANAGER_H

#include <list>
#include "Robot.h"

/****************************************************************************/
/*!
Class RobotManager:
\brief Defines the methods and variables for managing robots in the scene
*/
/****************************************************************************/

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