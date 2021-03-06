/****************************************************************************/
/*!
\file Boundary.h
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
Struct to define a Boundary in the scene
*/
/****************************************************************************/

#ifndef _BOUNDARY_H
#define _BOUNDARY_H

/****************************************************************************/
/*!
Struct Boundary:
\brief Defines 3D vectors and its methods
*/
/****************************************************************************/

struct Boundary
{
	Boundary(float xMin = 0, float xMax = 0, float zMin = 0, float zMax = 0, float yMin = 0, float yMax = 0, unsigned short type = 0) : xMin(xMin), xMax(xMax), zMin(zMin), zMax(zMax), yMin(yMin), yMax(yMax), type(type) {};
	Boundary(const Boundary& bound){};
	float xMin, xMax, zMin, zMax, yMin, yMax;
	// 0 for collidable, 1 for interactable;
	unsigned short type;
	bool active = true;
	bool isWithin = false;
	void set(float xMin = 0, float xMax = 0, float zMin = 0, float zMax = 0, float yMin = 0, float yMax = 0);
	bool BoundaryCheck(const float& playerCoordX, const float& playerCoordZ, const float& playerCoordY);
};

#endif //_BOUNDARY_H