#ifndef _BOUNDARY_H
#define _BOUNDARY_H

struct Boundary
{
	Boundary(float xMin = 0, float xMax = 0, float zMin = 0, float zMax = 0, unsigned short type = 0) : xMin(xMin), xMax(xMax), zMin(zMin), zMax(zMax), type(type) {};
	Boundary(const Boundary& bound){};
	float xMin, xMax, zMin, zMax, yMin, yMax;
	// 0 for collidable, 1 for interactable;
	unsigned short type;
	bool active = true;
	bool isWithin = false;
	void set(float xMin = 0, float xMax = 0, float zMin = 0, float zMax = 0, float yMin = 0, float yMax = 0);
	bool BoundaryCheck(const float& playerCoordX, const float& playerCoordZ);
};

#endif //_BOUNDARY_H