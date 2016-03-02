/****************************************************************************/
/*!
\file Raycast.cpp
\author Ryan Lim Rui An
\par email: 150577L@mymail.nyp.edu.sg
\brief
.cpp file that defines Raycast
*/
/****************************************************************************/
#include "RayCast.h"

/****************************************************************************/
/*!
\brief
Constructor
*/
/****************************************************************************/

RayCast::~RayCast()
{

}

/****************************************************************************/
/*!
\brief
Moves the RayCast
\param
		RayPos - determines the position of the RayCast
		StoredDirVec - returns the value of a stored direction vector
		Speed - determines the speed of the RayCast
\return
Resulting normalized vectors
*/
/****************************************************************************/

Vector3 RayCast::Move()
{
	//CalcIncrement(val);
	RayPos.x += StoredDirVec.x * Speed;
	RayPos.y += StoredDirVec.y * Speed;
	RayPos.z += StoredDirVec.z * Speed;
	CalcBounds();
	return RayPos;
}

/****************************************************************************/
/*!
\brief
Calculates the boundary of each RayCast
\param
		RayPos - determines the position of the RayCast
\return
Resulting normalized vectors
*/
/****************************************************************************/

void RayCast::CalcBounds()
{
	BoundingBox.set(RayPos.x - 3, RayPos.x + 3, RayPos.z - 3, RayPos.z + 3, RayPos.y - 3, RayPos.y + 3);
}

/****************************************************************************/
/*!
\brief
Calls the current position of each RayCast
\param
		RayPos - determines the position of the RayCast
\return
Resulting normalized vectors
*/
/****************************************************************************/

Vector3 RayCast::Position()
{
	return RayPos;
}