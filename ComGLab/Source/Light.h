/****************************************************************************/
/*!
\file Light.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Class to control light
*/
/****************************************************************************/
#ifndef LIGHT_H
#define LIGHT_H

#include "Vertex.h"

/****************************************************************************/
/*!
Struct Light:
\brief Defines the enums and methods for Light
*/
/****************************************************************************/

struct Light
{
	Light();
	Position position;
	Color color;
	float power;
	float kC, kL, kQ;

	enum LIGHT_TYPE
	{
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL,
		LIGHT_SPOT,
		LIGHT_TOTAL,
	};

	LIGHT_TYPE type;
	Vector3 spotDirection;
	float cosCutoff;
	float cosInner;
	float exponent;

};

#endif