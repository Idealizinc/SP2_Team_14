#include "SP2_Scene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

ISoundEngine* engine;

const std::string SoundName[] = {
	"Sounds//Spacebattle.mp3"
};

ISound* Song;

SP2_Scene::SP2_Scene()
{
}

SP2_Scene::~SP2_Scene()
{
}

void SP2_Scene::readtextfile()
{
	std::string line;
	std::ifstream startfile;
	startfile.open("ComGLab\readme\\start.txt");
	std::string file_contents;
	if (startfile.is_open())
	{
		while (std::getline(startfile, line))
		{
			file_contents += line;
			file_contents.push_back('\n');
			//std::cout << line << endl;
		}
		startfile.close();
	}
}

void SP2_Scene::Init()
{
	engine = createIrrKlangDevice();
	engine->addSoundSourceFromFile(SoundName[0].c_str());
	Song = engine->play2D(SoundName[0].c_str(), true, false, true);
	Song->setVolume(0.25);

	// Init VBO here
	glClearColor(0.15f, 0.2f, 0.35f, 0.0f);

	Mtx44 projection;
	projection.SetToPerspective(30.0f, static_cast < float >(S_Width) / static_cast < float >(S_Height), 0.1f, 3000.0f);
	projectionStack.LoadMatrix(projection);

	camera.Init(Vector3(0, 4, 8.5), Vector3(0, 4, 0), Vector3(0, 1, 0));

	//Initiallising Variables For Translate, Rotate, Scale
	tweenVal = 0;
	constRotation = 0;
	constTranslation = 0;
	DoorRot = 0;
	translateX = 5;
	scaleAll = 1;
	rotationalLimit = 180;
	translationLimit = 5;
	scalingLimit = 10;
	rLimiter = true;
	toggleLimiters = true;
	limitersON = true;
	lightOff = false;
	hp = 100;
	ammo = 100;
	wave = 1;
	state = 0;
	timer = 0;
	weaponValue = 0;
	weaponinterface = false;
	buttonPress = true;
	buttonValue = 0;
	robotCount = 0;
	pause = 1;

	// Enable depth Test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//Load Vertex and Fragment Shaders 
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	initLights();
	
	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	Color white(1, 1, 1);

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("axes", 1000, 1000, 1000);
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", white);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", white);
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", white, 36);
	meshList[GEO_RING] = MeshBuilder::GenerateRing("ring", white, 0.5, 36);
	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", white, 1, 0.5, 20, 20);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", white, 20);
	meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", white, 20);
	meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", white, 20, 20);

	//Skybox
	//Using the lower res skybox image
	SB_Day_front = LoadTGA("Image//Space_Front.tga");
	SB_Day_back = LoadTGA("Image//Space_Back.tga");
	SB_Day_top = LoadTGA("Image//Space_Top.tga");
	SB_Day_bottom = LoadTGA("Image//Space_Bottom.tga");
	SB_Day_left = LoadTGA("Image//Space_Left.tga");
	SB_Day_right = LoadTGA("Image//Space_Right.tga");

	//UI Elements
	Crosshair = LoadTGA("Image//Crosshair.tga");
	UI_BG = LoadTGA("Image//UI_BG_Black.tga");
	UI_HP_Red = LoadTGA("Image//UI_HP_Red.tga");
	UI_HP_Green = LoadTGA("Image//UI_HP_Green.tga");
	UI_WepSel_BG = LoadTGA("Image//UI_WepSel_BG.tga");
	Crosshair = LoadTGA("Image//Crosshair.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[GEO_FRONT]->textureID = SB_Day_front;
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[GEO_BACK]->textureID = SB_Day_back;
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[GEO_TOP]->textureID = SB_Day_top;
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[GEO_BOTTOM]->textureID = SB_Day_bottom;
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[GEO_LEFT]->textureID = SB_Day_left;
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[GEO_RIGHT]->textureID = SB_Day_right;

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_SMG] = MeshBuilder::GenerateOBJ("test", "OBJ//SMG.obj");
	meshList[GEO_SMG]->textureID = LoadTGA("Image//Tex_SMG.tga");

	meshList[GEO_SNIPER] = MeshBuilder::GenerateOBJ("test", "OBJ//Sniper.obj");
	meshList[GEO_SNIPER]->textureID = LoadTGA("Image//Tex_Sniper.tga");

	meshList[GEO_RIFLE] = MeshBuilder::GenerateOBJ("test", "OBJ//Rifle.obj");
	meshList[GEO_RIFLE]->textureID = LoadTGA("Image//Tex_Rifle.tga");

	meshList[GEO_SHOTGUN] = MeshBuilder::GenerateOBJ("test", "OBJ//Shotgun.obj");
	meshList[GEO_SHOTGUN]->textureID = LoadTGA("Image//Tex_Shotgun.tga");

	//meshList[GEO_PLAYERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//PlayerShip.obj");
	//meshList[GEO_PLAYERSHIP]->textureID = LoadTGA("Image//Tex_PlayerShip.tga");

	//meshList[GEO_MOTHERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership.obj");
	//meshList[GEO_MOTHERSHIP]->textureID = LoadTGA("Image//Tex_Mothership.tga");

	//meshList[GEO_DRONE] = MeshBuilder::GenerateOBJ("test", "OBJ//Drone.obj");
	//meshList[GEO_DRONE]->textureID = LoadTGA("Image//Tex_Drone.tga");

	//meshList[GEO_ROBOT1] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot1.obj");
	////meshList[GEO_SNIPER]->textureID = LoadTGA("Image//Tex_Robot1.tga");

	/*meshList[GEO_GATE] = MeshBuilder::GenerateOBJ("test", "OBJ//gate.obj");
	meshList[GEO_GATE]->textureID = LoadTGA("Image//gate.tga");*/

	/*meshList[GEO_METEOR] = MeshBuilder::GenerateOBJ("test", "OBJ//meteor.obj");
	meshList[GEO_METEOR]->textureID = LoadTGA("Image//meteor.tga");*/

	/*meshList[GEO_COMPUTER] = MeshBuilder::GenerateOBJ("test", "OBJ//computer.obj");
	meshList[GEO_COMPUTER]->textureID = LoadTGA("Image//computer.tga");*/

	initBounds();
}

void SP2_Scene::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 0.5f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);

}

void SP2_Scene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 160, 0, 90, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 0.55f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SP2_Scene::RenderImageOnScreen(GLuint texture, float Xsize, float Ysize, float Xpos, float Ypos)
{
	if (!texture) //Proper error check
		return;
	Mesh* mesh = meshList[GEO_QUAD];
	mesh->textureID = texture;
	glDisable(GL_DEPTH_TEST);
	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 160, 0, 90, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(Xpos, Ypos, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(Xsize, 1, Ysize);
	RenderMesh(meshList[GEO_AXES], false);
	RenderMesh(mesh, false);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}


void SP2_Scene::RenderWeaponInHand(unsigned short wepVal, float size, float x, float y)
{
	Mtx44 ortho;
	ortho.SetToOrtho(0, 170, 0, 90, -70, 140); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(140, 5, -10);
	modelStack.Rotate(200, 0, 1, 0);
	modelStack.Rotate(10, -1, 0, 0);
	modelStack.Scale(20, 20, 20);
	if (wepVal == 0)
	{
		RenderMesh(meshList[GEO_SMG], true);
	}
	if (wepVal == 1)
	{
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	if (wepVal == 2)
	{
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	if (wepVal == 3)
	{
		RenderMesh(meshList[GEO_SHOTGUN], true);
	}
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
}


void SP2_Scene::RenderMeshOnScreen(Mesh* mesh, float Xsize, float Ysize, float Xpos, float Ypos, float Angle, Vector3 RotationDir)
{
	Mtx44 ortho;
	ortho.SetToOrtho(0, 170, 0, 90, -70, 140); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(Xpos, Ypos, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(Angle, RotationDir.x, RotationDir.y, RotationDir.z);
	modelStack.Scale(3, 3, 3);
	modelStack.Scale(Xsize, Ysize, Xsize);
	RenderMesh(mesh, true);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
}

void SP2_Scene::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();

		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once 
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void SP2_Scene::initLights()
{
	//Light 0 - Sun
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	lightDefaultPos.Set(0, 500, 10);

	glUseProgram(m_programID);
	light[0].type = Light::LIGHT_DIRECTIONAL;
	light[0].position.Set(lightDefaultPos.x, lightDefaultPos.y, lightDefaultPos.z);
	light[0].color.Set(1, 0.95, 1);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.0f;
	light[0].spotDirection.Set(0.0f, 1.0f, 0.0f);

	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);
}

void SP2_Scene::initBounds()
{
	
}

void SP2_Scene::gamestate()
{
	if (wave == 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 1 clear", Color(1, 0, 0), 3, 20, 15); //appear for a few seconds

		if (robotCount == 0)
		{
			weaponinterface == true;
		}
		else if (hp == 0)
		{
			//go back to start screen
		}
	}
	if (wave == 2)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 2 clear", Color(1, 0, 0), 3, 20, 15);

		if (robotCount == 0)
		{
			weaponinterface == true;
		}
		else if (hp == 0)
		{
			//go back to start screen
		}
	}
	if (wave == 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 3 clear", Color(1, 0, 0), 3, 20, 15);

		if (robotCount == 0)
		{
			weaponinterface == true;
		}
		else if (hp == 0)
		{
			//go back to start screen
		}
	}
	if (wave == 4)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 4 clear", Color(1, 0, 0), 3, 20, 15);

		if (robotCount == 0)
		{
			weaponinterface == true;
		}
		else if (hp == 0)
		{
			//go back to start screen
		}
	}
	if (wave == 5)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 5 clear", Color(1, 0, 0), 3, 20, 15);

		if (robotCount == 0)
		{
			weaponinterface == true;
		}
		else if (hp == 0)
		{
			//go back to start screen
		}
	}
	if (wave == 6)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Boss Stage", Color(1, 0, 0), 3, 20, 15);
		//if (//boss dead)
		//{

		//}
		//else if (hp == 0)
		//{
		//	//go back to start screen
		//}
	}
}
void SP2_Scene::Update(double dt)
{
	camera.Update(dt);
	constRotation += (float)(10 * pause * dt);
	constTranslation += (float)(10 * pause  * dt);
	//Lerping Rotation
	if ((rLimiter == true))
	{
		tweenVal += (float)(50 * pause  * dt);
	}
	else if ((rLimiter == false))
	{
		tweenVal -= (float)(50 * pause * dt);
	}
	if (tweenVal >= rotationalLimit)
	{
		rLimiter = false;
	}
	else if (tweenVal <= -rotationalLimit + (180 - rotationalLimit))
	{
		rLimiter = true;
	}
	//End

	//Wrapping Translation
	if (translateX >= translationLimit)
	{
		translateX = -40;
	}
	else if (translateX <= -translationLimit)
	{
		translateX = 40;
	}
	translateX += (float)(10 * pause  * dt /* EXTRA */ * 2);
	//End

	//Resetting Scaling
	if (scaleAll >= scalingLimit)
	{
		scaleAll = 5;
	}
	scaleAll += (float)(2 * pause  * dt);
	if (Application::IsKeyPressed('6'))
	{
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('7'))
	{
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('8'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (Application::IsKeyPressed('9'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//Weapon
	if (weaponinterface == true)
	{
		if (buttonPress == true && Application::IsKeyPressed('1'))
		{
			weaponValue = 1;
			wave += 1;
			buttonPress == false;
			buttonValue = 0;
			weaponinterface = false;
		}
		else if (buttonPress == true && Application::IsKeyPressed('2'))
		{
			weaponValue = 2;
			wave += 1;
			buttonPress == false;
			buttonValue = 0;
			weaponinterface = false;
		}
		else if (buttonPress == true && Application::IsKeyPressed('3'))
		{
			weaponValue = 3;
			wave += 1;
			buttonPress == false;
			buttonValue = 0;
			weaponinterface = false;
		}
	}

	if (!buttonPress)
	{
		buttonValue += 1 * pause * dt;
		if (buttonValue >= 10)
		{
			buttonPress = true;
		}
	}

	if (Application::IsKeyPressed('I'))
	{
		weaponinterface = true;
	}

	//timer += (float)(1 * dt);

	if (Application::IsKeyPressed('P') && pause == 1)
	{
		pause = 0;
		buttonPress == false;
		buttonValue = 0;
	}
	else if (Application::IsKeyPressed('O') && pause == 0)
	{
		pause = 1;
		buttonPress == false;
		buttonValue = 0;
	}

	framesPerSecond = 1 * pause / dt;

	TownLightPosition.y += tweenVal / 15000;
	light[1].position.Set(TownLightPosition.x, TownLightPosition.y, TownLightPosition.z);
	RoomLightPosition.y += tweenVal / 150000;
	light[2].position.Set(RoomLightPosition.x, RoomLightPosition.y, RoomLightPosition.z);
}

void SP2_Scene::RenderSkybox(Vector3 Position)
{
	float scaleSB = 2500;
	float transSB = scaleSB / 2 - scaleSB / 1250;
	//Skybox
	modelStack.PushMatrix();
	//Skybox Movement
	modelStack.Translate(Position.x, 0, Position.z);
	//GEO_FRONT
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -transSB);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(scaleSB, scaleSB, scaleSB);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();
	//GEO_BACK
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, transSB);
	modelStack.Rotate(90, -1, 0, 0);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(scaleSB, scaleSB, scaleSB);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();
	//GEO_TOP
	modelStack.PushMatrix();
	modelStack.Translate(0, transSB, 0);
	modelStack.Rotate(180, 1, 0, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(scaleSB, scaleSB, scaleSB);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();
	//GEO_BOTTOM
	modelStack.PushMatrix();
	modelStack.Translate(0, -transSB, 0);
	modelStack.Scale(scaleSB, scaleSB, scaleSB);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();
	//GEO_LEFT
	modelStack.PushMatrix();
	modelStack.Translate(-transSB, 0, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(90, 0, 0, -1);
	modelStack.Scale(scaleSB, scaleSB, scaleSB);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();
	//GEO_RIGHT
	modelStack.PushMatrix();
	modelStack.Translate(transSB, 0, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Scale(scaleSB, scaleSB, scaleSB);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
	modelStack.PopMatrix();
}

void SP2_Scene::RenderWepScreen(bool render, Vector3 choices)
{
	if (render){
		//Wep Select UI - Overall
		modelStack.PushMatrix();
		RenderImageOnScreen(UI_WepSel_BG, 130, 65, 80, 45);
		//Left
		RenderImageOnScreen(UI_BG, 30, 30, 40, 40);
		RenderMeshOnScreen(meshList[GEO_RIFLE], 1, 1, 40, 40, constRotation * pause * 5, Vector3(1, 1, 0));
		//Center
		RenderImageOnScreen(UI_BG, 30, 30, 80, 40);
		RenderMeshOnScreen(meshList[GEO_SNIPER], 1, 1, 85, 40, constRotation * pause * 5, Vector3(1, 1, 0));
		//Right
		RenderImageOnScreen(UI_BG, 30, 30, 120, 40);
		RenderMeshOnScreen(meshList[GEO_SHOTGUN], 1, 1, 125, 40, constRotation * pause * 5, Vector3(1, 1, 0));
		modelStack.PopMatrix();
		//Wep Select UI END
	}
}

void SP2_Scene::Render(double dt)
{
	// Render VBO here
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);

	readtextfile();

	modelStack.LoadIdentity();

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightposition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightposition_cameraspace.x);
	}
	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightposition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightposition_cameraspace.x);
	}

	if (lightOff == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
		//Hide the lightball
		//RenderMesh(meshList[GEO_LIGHTBALL], false);
		modelStack.PopMatrix();
	}
	if (skyboxID == 0)
	{
		RenderSkybox(camera.getCameraPosition());
	}

	if (wave == 0)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Robot wave incoming", Color(1, 0, 0), 3, 60, 87);
	}
	if (wave == 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "*Robots coming from all directions*", Color(1, 0, 0), 3, 60, 87);
	}
	if (wave == 2)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Robots now walk faster!!", Color(1, 0, 0), 3, 60, 87);
	}
	if (wave == 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Meteors incoming!!!", Color(1, 0, 1), 3, 60, 87);
	}
	if (wave == 4)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Robots now fire weapons faster!!", Color(1, 0, 0), 3, 60, 87);
	}
	if (wave == 5)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Robots are now in god mode!!", Color(1, 0, 0), 3, 60, 87);
	}
	//boss wave 
	if (wave == 6)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Boss: Defeat Mothership", Color(1, 0, 0), 3, 60, 87);
	}
	readtextfile();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(constTranslation, 2, 5);
	modelStack.Scale(0.8, 0.8, 0.8);
	RenderMesh(meshList[GEO_SNIPER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderWeaponInHand(weaponValue, 5, 1, 1);
	modelStack.PopMatrix();

	//INFO UI, STATS - BOTTOM LEFT
	modelStack.PushMatrix();
	RenderImageOnScreen(UI_BG, 50, 10, 25, 5);
	std::stringstream fpsText;
	fpsText << std::fixed << std::setprecision(1) << "FPS = " << framesPerSecond;
	RenderTextOnScreen(meshList[GEO_TEXT], fpsText.str(), Color(1, 1, 1), 2.5, 5, 7.5);
	std::stringstream coordText;
	coordText << std::fixed << std::setprecision(1) << "Player Location = (" << camera.getCameraPosition().x << ", " << camera.getCameraPosition().y << ", " << camera.getCameraPosition().z << ")";
	RenderTextOnScreen(meshList[GEO_TEXT], coordText.str(), Color(1, 1, 1), 2.5, 5, 5);
	modelStack.PopMatrix();
	//INFO UI, STATS END

	//INFO UI, HP - CENTER
	modelStack.PushMatrix();
	RenderImageOnScreen(UI_BG, 50, 10, 80, 5);
	modelStack.PopMatrix();
	//INFO UI, HP END

	RenderImageOnScreen(Crosshair, 10, 10, 80, 45);
	
	RenderWepScreen(weaponinterface);

	RenderTextOnScreen(meshList[GEO_TEXT], "Base HP: " + std::to_string(hp), Color(0, 0.5, 0), 3, 2.5, 87);

	RenderTextOnScreen(meshList[GEO_TEXT], "Ammo: " + std::to_string(ammo), Color(0, 0.5, 0), 3, 2.5, 84);

	RenderTextOnScreen(meshList[GEO_TEXT], "Wave Number: " + std::to_string(wave), Color(0, 0.5, 0), 3, 2.5, 81);

	if (pause == 0)
	{
		modelStack.PushMatrix();
		RenderImageOnScreen(UI_BG, 50, 10, 80, 45);
		RenderTextOnScreen(meshList[GEO_TEXT], "Paused", Color(0, 0.5, 0), 10, 68, 45);
		modelStack.PopMatrix();
	}
}

void SP2_Scene::Exit()
{
	engine->drop();
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

