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