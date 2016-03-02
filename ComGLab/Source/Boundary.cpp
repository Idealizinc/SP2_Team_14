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

/****************************************************************************/
/*!
\brief
Sets the minimum and maximum x,y and z boundaries.
\param 
		xMin - miminum point for x-axis
		xMax - maximum point for x-axis
		zMin - minimum point for z-axis
		zMax - maxinum point for z-axis
		yMin - minimum point for y-axis
		yMax - maximum point for y-axis
\return
values for boundaries
*/
/****************************************************************************/

void Boundary::set(float xMin, float xMax, float zMin, float zMax, float yMin, float yMax)
{
	this->xMin = xMin;
	this->xMax = xMax;
	this->zMin = zMin;
	this->zMax = zMax;
	this->yMin = yMin;
	this->yMax = yMax;
}

/****************************************************************************/
/*!
\brief
Checks if the players position is outside of the boundaries set
\param 
		xMin - miminum point for x-axis
		xMax - maximum point for x-axis
		zMin - minimum point for z-axis
		zMax - maxinum point for z-axis
		yMin - minimum point for y-axis
		yMax - maximum point for y-axis
		playerCoordX - Player's position on the x-axis
		playerCoordZ - Player's position on the z-axis
		playerCoordY - Player's position on the y-axis
\return
Returns false if player is outside of the boundaries
*/
/****************************************************************************/

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