#include "Robot.h"
Robot::~Robot()
{

}
Vector3 Robot::Move()
{
	RobotPos.x += 1 * Speed;
	RobotPos.y += StoredDirVec.y * Speed;
	RobotPos.z += 1 * Speed;
	return RobotPos;
}
Vector3 Robot::Position()
{
	return RobotPos;
}
