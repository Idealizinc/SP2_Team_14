#include "Material.h"

Material::Material()
{
	kAmbient.Set(0.4f, 0.4f, 0.4f);
	kDiffuse.Set(0.5f, 0.5f, 0.5f);
	kSpecular.Set(0.3f, 0.3f, 0.3f);
	kShininess = 0.3f;
}