/****************************************************************************/
/*!
\file Light.cpp
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
.cpp file that defines Light
*/
/****************************************************************************/
#include "Light.h"

/****************************************************************************/
/*!
\brief
Determines the variables of light
\param
		position - Vector3 that determines the position of the light
		color - determines the color of the light
		power - determines the power of the light
\return
Resulting values and normalized vectors
*/
/****************************************************************************/

Light::Light()
{
	position.Set(0, 0, 0);
	color.Set(1.0f, 1.0f, 1.0f);
	power = 1;
	kC = kL = kQ = 0.0f;
}