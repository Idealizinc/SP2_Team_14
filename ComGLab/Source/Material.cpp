/****************************************************************************/
/*!
\file Camera.cpp
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
.cpp file that defines default light properties
*/
/****************************************************************************/
#include "Material.h"

Material::Material()
{
	kAmbient.Set(0.4f, 0.4f, 0.4f);
	kDiffuse.Set(0.5f, 0.5f, 0.5f);
	kSpecular.Set(0.3f, 0.3f, 0.3f);
	kShininess = 0.3f;
}