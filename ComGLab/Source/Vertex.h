/****************************************************************************/
/*!
\file Vertex.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Header file for vertex
*/
/****************************************************************************/
#ifndef VERTEX_H
#define VERTEX_H

#include "Vector3.h"

/****************************************************************************/
/*!
Struct Position:
\brief Defines the methods and variables for setting position
*/
/****************************************************************************/

struct Position
{
	float x, y, z;
	Position(float x = 0, float y = 0, float z = 0)
	{
		Set(x, y, z);
	};
	void Set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
};

/****************************************************************************/
/*!
Struct Color:
\brief Defines the methods and variables for setting color
*/
/****************************************************************************/

struct Color
{
	float r, g, b;
	Color(float r = 1, float g = 0, float b = 1)
	{ 
		Set(r, g, b);
	};
	void Set(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
};

/****************************************************************************/
/*!
Struct TexCoord:
\brief Defines the methods and variables for setting TexCoord
*/
/****************************************************************************/

struct TexCoord
{
	float u, v;
	TexCoord(float u = 0, float v = 0) { Set(u, v); }
	void Set(float u, float v) { this->u = u; this->v = v; }
};

/****************************************************************************/
/*!
Struct Vertex:
\brief Defines the methods for Vertex
*/
/****************************************************************************/

struct Vertex
{
	Position pos;
	Color color;
	Vector3 normal;
	TexCoord texCoord;
};

#endif