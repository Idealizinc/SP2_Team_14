/****************************************************************************/
/*!
\file Boundary.cpp
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
.cpp file that defines a boundary in the scene
*/
/****************************************************************************/
#include "Boundary.h"

void Boundary::set(float xMin, float xMax, float zMin, float zMax, float yMin, float yMax)
{
	this->xMin = xMin;
	this->xMax = xMax;
	this->zMin = zMin;
	this->zMax = zMax;
	this->yMin = yMin;
	this->yMax = yMax;
}

bool Boundary::BoundaryCheck(const float& playerCoordX, const float& playerCoordZ, const float& playerCoordY) {
	if (xMax > playerCoordX &&
		xMin < playerCoordX &&
		zMax > playerCoordZ &&
		zMin < playerCoordZ &&
		yMax > playerCoordY &&
		yMin < playerCoordY)
	{
		return false;
	}
	return true;
}