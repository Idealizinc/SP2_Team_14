/****************************************************************************/
/*!
\file Material.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Struct that defines light properties
*/
/****************************************************************************/
#ifndef _MATERIAL_H
#define _MATERIAL_H

/****************************************************************************/
/*!
Struct Component:
\brief Defines the methods and variables for light properties
*/
/****************************************************************************/

struct Component
{
	float r, g, b;
	Component(float r = 0.1f, float g = 0.1f, float b = 0.1f){ Set(r, g, b); };
	void Set(float r, float g, float b){ this->r = r, this->g = g, this->b = b; };
};

/****************************************************************************/
/*!
Struct Material:
\brief Defines the methods and variables for light properties
*/
/****************************************************************************/

struct Material
{
	Material();
	Component kAmbient;
	Component kDiffuse;
	Component kSpecular;
	float kShininess;
};

#endif