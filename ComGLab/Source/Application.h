/****************************************************************************/
/*!
\file Application.h
\author Wen Sheng Tang
\par email: tang_wen_sheng\@nyp.edu.sg
\brief
Header file that contains the class to run the application
*/
/****************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

/****************************************************************************/
/*!
Class Application:
\brief Defines the methods used to run the Application and define key pressed
*/
/****************************************************************************/

class Application
{
public:
	Application();
	~Application();
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static bool CloseGame;

private:
	//Declare a window object
	StopWatch m_timer;
};

#endif