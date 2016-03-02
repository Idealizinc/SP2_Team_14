/****************************************************************************/
/*!
\file Scene.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Class to defines scene
*/
/****************************************************************************/
#ifndef SCENE_H
#define SCENE_H

/****************************************************************************/
/*!
Class Scene:
\brief Defines the methods for Scene
*/
/****************************************************************************/

class Scene
{
public:
	Scene() {}
	~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render(double dt) = 0;
	virtual void Exit() = 0;
};

#endif