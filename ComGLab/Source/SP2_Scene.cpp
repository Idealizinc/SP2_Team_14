/****************************************************************************/
/*!
\file SP2_Scene.cpp
\author Ryan Lim Rui An, Teh Kai Hong Bryan, Andy Ong Jian Yao
\par email: 150577L@mymail.nyp.edu.sg, 150587A@mymail.nyp.edu.sg, 152581X@mymail.nyp.edu.sg
\brief
.cpp file that defines scenes
*/
/****************************************************************************/

#include "SP2_Scene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"
#include "Application.h"
#include "MeshBuilder.h"
#include "Robot.h"
#include "Utility.h"
#include <fstream>
#include <iostream>
#include <sstream>

using std::stringstream;
using std::cout;
using std::endl;

ISoundEngine* engine;

const std::string SoundName[] = {
	"Sounds//SFX_Laser.ogg",
	"Sounds//Gnosemite.ogg",
	"Sounds//Cobra2.ogg",
	"Sounds//Viper_Orchestral.ogg",
	"Sounds//SFX_Explosion.ogg",
};

ISound* ShootSound;
ISound* WaveBGM;
ISound* WaveBGM2;
ISound* MenuBGM;
ISound* LaserHit;
ISound* ExplosionSound;

int pause = 1;


/****************************************************************************/
/*!
\brief
Constructor
*/
/****************************************************************************/

SP2_Scene::SP2_Scene()
{
}


/****************************************************************************/
/*!
\brief
Destructor
*/
/****************************************************************************/

SP2_Scene::~SP2_Scene()
{
}


/****************************************************************************/
/*!
\brief
Reads a text file
*/
/****************************************************************************/

void SP2_Scene::readtextfile()
{
	std::string line;
	std::ifstream startfile;
	startfile.open("ComGLab\\readme\\start.txt");
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


/****************************************************************************/
/*!
\brief
Initializes values
*/
/****************************************************************************/

void SP2_Scene::Init()
{
	engine = createIrrKlangDevice();
	engine->addSoundSourceFromFile(SoundName[0].c_str());
	engine->addSoundSourceFromFile(SoundName[1].c_str());
	engine->addSoundSourceFromFile(SoundName[2].c_str());
	engine->addSoundSourceFromFile(SoundName[3].c_str());
	engine->addSoundSourceFromFile(SoundName[4].c_str());

	// Init VBO here
	glClearColor(0.15f, 0.2f, 0.35f, 0.0f);

	Mtx44 projection;
	projection.SetToPerspective(40.0f, static_cast < float >(S_Width) / static_cast < float >(S_Height), 0.1f, 3000.0f);
	projectionStack.LoadMatrix(projection);

	camera.Init(Vector3(0, 5, 3), Vector3(0, 5, 0), Vector3(0, 1, 0), 20);
	
	// Enable depth Test
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
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

	initUIElements();

	Color white(1, 1, 1);

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("axes", 1000, 1000, 1000);
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", white);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", white);
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", white, 36);
	meshList[GEO_RING] = MeshBuilder::GenerateRing("ring", white, 0.5, 36);
	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", white, 1, 0.1, 20, 20);
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", white, 20);
	meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", white, 20);
	meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", white, 20, 20);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", white, 20, 20);

	//InitWeaponModels();
	//InitMapModels();
	//InitRobots();
}

void SP2_Scene::initUIElements()
{
	//UI Elements
	Crosshair = LoadTGA("Image//Crosshair.tga");
	UI_BG = LoadTGA("Image//UI_BG_Black.tga");
	UI_HP_Red = LoadTGA("Image//UI_HP_Red.tga");
	UI_HP_Green = LoadTGA("Image//UI_HP_Green.tga");
	UI_WepSel_BG = LoadTGA("Image//UI_WepSel_BG.tga");
	UI_LoadingBG = LoadTGA("Image//UI_LoadScreen.tga");
	UI_LoadingBarOverlay = LoadTGA("Image//UI_HP_Overlay.tga");
	UI_Logo = LoadTGA("Image//UI_GameLogo.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//ExportedFont.tga");

	//Skybox
	//Using the lower res skybox image
	SB_Day_front = LoadTGA("Image//Space_Front.tga");
	SB_Day_back = LoadTGA("Image//Space_Back.tga");
	SB_Day_top = LoadTGA("Image//Space_Top.tga");
	SB_Day_bottom = LoadTGA("Image//Space_Bottom.tga");
	SB_Day_left = LoadTGA("Image//Space_Left.tga");
	SB_Day_right = LoadTGA("Image//Space_Right.tga");

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
	
}


/****************************************************************************/
/*!
\brief
Holds all the variables used
\param
		tweenVal - value that determines tween
		constRotation - value that constantly changes with dt
		constTranslation - value that constantly changes with dt
		translateX - value that translates along the x-axis
		scaleAll - value used for scaling
		rLimiter - boolean used for animation
		toggleLimiters - boolean used to toggle limiters on 
		limitersOn - boolean that checks if limiters are on 
		lightsOff - boolean that checks if lights are on 
		basehp - value for base hp
		bosshp - value for boss hp
		playerhp - value for player hp
		GameOver - boolean that checks if the game is over 
		wave - value that determines what wave the game is in
		weaponValue - value that determines what gun is used
		weaponinterface - boolean that determines if the interface is on screen
		buttonPress - boolean that checks if a button is pressed or not
		buttonValue - value that creates a delay between one button press and the next
		ShipX - value used for translating the drivable ship along the x-axis
		ShipY - value used for translating the drivable ship along the y-axis
		ShipRot - value used for rotating the drivable ship
		repairShipPhase - value used to determine which phase of the repair ship quest the game is in
		moveRocks - value used to translate rocks
		floatRocks - boolean that determines if rocks are floating
		ShipRotation - value used for rotating enemy mothership
		Intro - boolean that checks if the introductory cutscene is playing
		weaponsOn - boolean that determines if the mothership is firing at the player
		ShipSpawned - boolean that determines if the enemy mothership has spawned
		explosion - value used for animation
		PowerUpActive - boolean that determines if a Power Up is active
		WepItf_Choices - Vector3 value for the weapon interface screen
		SpawnPointN - Vector3 value for a robot spawn point
		SpawnPointS - Vector3 value for a robot spawn point
		SpawnPointE - Vector3 value for a robot spawn point
		SpawnPointW - Vector3 value for a robot spawn point
*/
/****************************************************************************/

void SP2_Scene::InitKeyVariables()
{
	tweenVal = 0;
	constRotation = 0;
	constTranslation = 0;
	translateX = 0;
	scaleAll = 1;
	rotationalLimit = 180;
	translationLimit = 5;
	scalingLimit = 10;
	rLimiter = true;
	toggleLimiters = true;
	limitersON = true;
	lightOff = false;
	basehp = 100;
	bosshp = 100;
	playerhp = 100;
	GameOver = false;
	wave = 1;
	weaponValue = 0;
	weaponinterface = false;
	buttonPress = true;
	buttonValue = 0;
	ShipX = -20;
	ShipY = 150;
	ShipRot = 0;
	repairShipPhase = 0;
	moveRocks = 0;
	floatRocks = true;
	ShipRotation = 90;
	Intro = false;
	weaponsOn = false;
	ShipSpawned = false;
	explosion = 0;
	PowerUpActive = true;
	playOnce = false;

	//robotleftattack = false;
	//robotrightattack = false;
	WepItf_Choices = Vector3(0, 0, 0);

	spawnPointN = Vector3(0, 0, 250);
	spawnPointS = Vector3(0, 0, -250);
	spawnPointE = Vector3(-250, 0, 0);
	spawnPointW = Vector3(250, 0, 0);

	camera.maxCameraXrotation = 89;
	camera.minCameraXrotation = -89;
	camera.maxCameraYrotation = 360;
	camera.minCameraYrotation = -360;

	WepSys.ClearList();
	EnemyWepSys.ClearList();
	RobotManager.ClearList();
	MothershipHandler.ClearList();
}

/****************************************************************************/
/*!
\brief
Initializes OBJ and TGA files
*/
/****************************************************************************/

void SP2_Scene::InitWeaponModels()
{
	Normal_Sniper = LoadTGA("Image//Tex_Sniper.tga");
	Normal_Rifle = LoadTGA("Image//Tex_Rifle.tga");
	Normal_SMG = LoadTGA("Image//Tex_SMG.tga");
	meshList[GEO_PLAYERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//PlayerShip.obj");
	meshList[GEO_PLAYERSHIP]->textureID = LoadTGA("Image//Tex_PlayerShip.tga");

	// Damage
	Damage_Sniper = LoadTGA("Image//Tex_SniperDamage.tga");
	Damage_Rifle = LoadTGA("Image//Tex_RifleDamage.tga");
	Damage_SMG = LoadTGA("Image//Tex_SMGDamage.tga");

	// Ammo
	Capacity_Sniper = LoadTGA("Image//Tex_SniperAmmo.tga");
	Capacity_Rifle = LoadTGA("Image//Tex_RifleAmmo.tga");
	Capacity_SMG = LoadTGA("Image//Tex_SMGAmmo.tga");

	// Fast
	Fast_Sniper = LoadTGA("Image//Tex_SniperFast.tga");
	Fast_Rifle = LoadTGA("Image//Tex_RifleFast.tga");
	Fast_SMG = LoadTGA("Image//Tex_SMGFast.tga");

	Bullet_Red = LoadTGA("Image//Tex_Bullet_Red.tga");
	Bullet_Teal = LoadTGA("Image//Tex_Bullet_LBlue.tga");
	Bullet_Blue = LoadTGA("Image//Tex_Bullet_Blue.tga");
	Bullet_Green = LoadTGA("Image//Tex_Bullet_Green.tga");
	Bullet_White = LoadTGA("Image//Tex_Bullet_White.tga");

	meshList[GEO_BULLET] = MeshBuilder::GenerateOBJ("test", "OBJ//Bullet.obj");
	meshList[GEO_BULLET]->textureID = Bullet_White;

	meshList[GEO_ENEMYBULLET] = MeshBuilder::GenerateOBJ("test", "OBJ//Bullet.obj");
	meshList[GEO_ENEMYBULLET]->textureID = Bullet_Red;

	meshList[GEO_SMG] = MeshBuilder::GenerateOBJ("test", "OBJ//SMG.obj");
	meshList[GEO_SMG]->textureID = LoadTGA("Image//Tex_SMG.tga");

	meshList[GEO_SNIPER] = MeshBuilder::GenerateOBJ("test", "OBJ//Sniper.obj");
	meshList[GEO_SNIPER]->textureID = LoadTGA("Image//Tex_Sniper.tga");

	meshList[GEO_RIFLE] = MeshBuilder::GenerateOBJ("test", "OBJ//Rifle.obj");
	meshList[GEO_RIFLE]->textureID = LoadTGA("Image//Tex_Rifle.tga");
}

/****************************************************************************/
/*!
\brief
Initializes OBJ and TGA files
*/
/****************************************************************************/

void SP2_Scene::InitMapModels()
{
	meshList[GEO_POWERUP] = MeshBuilder::GenerateOBJ("test", "OBJ//Bullet.obj");
	meshList[GEO_POWERUP]->textureID = Bullet_White;

	meshList[GEO_CRYSTALBASE] = MeshBuilder::GenerateOBJ("Core Platform", "OBJ//Core_Platform.obj");
	meshList[GEO_CRYSTALBASE]->textureID = LoadTGA("Image//Tex_Core_Platform.tga");

	meshList[GEO_CRYSTAL] = MeshBuilder::GenerateOBJ("Core Crystal", "OBJ//Core_Crystal.obj");
	meshList[GEO_CRYSTAL]->textureID = LoadTGA("Image//Tex_LightOrb.tga");

	meshList[GEO_MOONFLOOR] = MeshBuilder::GenerateOBJ("Baseplate", "OBJ//Moon_Floor.obj");
	meshList[GEO_MOONFLOOR]->textureID = LoadTGA("Image//Tex_Moon.tga");

	meshList[GEO_TELEPORTER] = MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter.obj");
	meshList[GEO_TELEPORTER]->textureID = LoadTGA("Image//Tex_Lightorb.tga");

	meshList[GEO_BASE] = MeshBuilder::GenerateOBJ("base", "OBJ//base.obj");
	meshList[GEO_BASE]->textureID = LoadTGA("Image//Tex_Gate2.tga");

	meshList[GEO_METEOR] = MeshBuilder::GenerateOBJ("test", "OBJ//meteor.obj");
	meshList[GEO_METEOR]->textureID = LoadTGA("Image//Tex_Meteor.tga");

	//meshList[GEO_PLAYERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//PlayerShip.obj");
	//meshList[GEO_PLAYERSHIP]->textureID = LoadTGA("Image//Tex_PlayerShip.tga");

	meshList[GEO_MOTHERSHIP_BOTTOM] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership_bottom.obj");
	meshList[GEO_MOTHERSHIP_BOTTOM]->textureID = LoadTGA("Image//Tex_Mothership.tga");

	meshList[GEO_MOTHERSHIP_LOWER] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership_lowerbody.obj");
	meshList[GEO_MOTHERSHIP_LOWER]->textureID = meshList[GEO_MOTHERSHIP_BOTTOM]->textureID;

	meshList[GEO_MOTHERSHIP_TAIL] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership_tail.obj");
	meshList[GEO_MOTHERSHIP_TAIL]->textureID = meshList[GEO_MOTHERSHIP_BOTTOM]->textureID;

	meshList[GEO_MOTHERSHIP_TOP] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership_top.obj");
	meshList[GEO_MOTHERSHIP_TOP]->textureID = meshList[GEO_MOTHERSHIP_BOTTOM]->textureID;

	meshList[GEO_MOTHERSHIP_UPPER] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership_upperbody.obj");
	meshList[GEO_MOTHERSHIP_UPPER]->textureID = meshList[GEO_MOTHERSHIP_BOTTOM]->textureID;

	/*meshList[GEO_COMPUTER] = MeshBuilder::GenerateOBJ("test", "OBJ//computer.obj");
	meshList[GEO_COMPUTER]->textureID = LoadTGA("Image//computer.tga");*/
}

/****************************************************************************/
/*!
\brief
Initializes OBJ and TGA files
*/
/****************************************************************************/

void SP2_Scene::InitRobots()
{
	meshList[GEO_ROBOTHEALTH] = MeshBuilder::GenerateCube("cube", Color(0, 1, 0));
	meshList[GEO_ROBOTHEALTH2] = MeshBuilder::GenerateCube("cube", Color(1, 0, 0));

	//range robot
	meshList[GEO_ROBOTBODY] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_body.obj");
	GLuint texGD2 = LoadTGA("Image//Tex_Robot2.tga");
	meshList[GEO_ROBOTBODY]->textureID = texGD2;
	meshList[GEO_ROBOTLEFTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_leftupperarm.obj");
	meshList[GEO_ROBOTLEFTUPPERARM]->textureID = texGD2;
	meshList[GEO_ROBOTLEFTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_leftlowerarm.obj");
	meshList[GEO_ROBOTLEFTLOWERARM]->textureID = texGD2;
	meshList[GEO_ROBOTLEFTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_leftleg.obj");
	meshList[GEO_ROBOTLEFTLEG]->textureID = texGD2;
	meshList[GEO_ROBOTRIGHTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_rightupperarm.obj");
	meshList[GEO_ROBOTRIGHTUPPERARM]->textureID = texGD2;
	meshList[GEO_ROBOTRIGHTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_rightlowerarm.obj");
	meshList[GEO_ROBOTRIGHTLOWERARM]->textureID = texGD2;
	meshList[GEO_ROBOTRIGHTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//Robot_rightleg.obj");
	meshList[GEO_ROBOTRIGHTLEG]->textureID = texGD2;
}

/****************************************************************************/
/*!
\brief
Method that renders text in the scene
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Method that renders text on screen
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Method that renders images in the scene
*/
/****************************************************************************/

void SP2_Scene::RenderImageInMap(GLuint texture, float Xsize, float Ysize)
{
	if (!texture) //Proper error check
		return; 
	Mesh* mesh = meshList[GEO_QUAD];
	mesh->textureID = texture;
	
	modelStack.PushMatrix();
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(Xsize, 1, Ysize);
	RenderMesh(mesh, false);
	modelStack.PopMatrix();
}

/****************************************************************************/
/*!
\brief
method that renders images on screen
*/
/****************************************************************************/

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
	//RenderMesh(meshList[GEO_AXES], false);
	RenderMesh(mesh, false);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

/****************************************************************************/
/*!
\brief
Method that renders a weapon on screen
*/
/****************************************************************************/

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
	if (wepVal >= 0 && wepVal <= 12)
	{
		modelStack.Translate(140, 4, -10);
		modelStack.Rotate(200, 0, 1, 0);
		modelStack.Rotate(10, -1, 0, 0);
		modelStack.Scale(20, 20, 20);
	}
	else if (wepVal == 13)
	{
		modelStack.Translate(140, 4, -10);
		modelStack.Rotate(200, 0, 1, 0);
		modelStack.Rotate(10, -1, 0, 0);
		modelStack.Scale(20, 20, 20);
		modelStack.Translate(0, -4, 0);
	}
	if (wepVal == 0)
	{
		meshList[GEO_BULLET]->textureID = Bullet_White;
		meshList[GEO_SMG]->textureID = Normal_SMG;
		RenderMesh(meshList[GEO_SMG], true);
	}
	else if (wepVal == 1)
	{
		meshList[GEO_BULLET]->textureID = Bullet_White;
		meshList[GEO_RIFLE]->textureID = Normal_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 2)
	{
		meshList[GEO_BULLET]->textureID = Bullet_White;
		meshList[GEO_SNIPER]->textureID = Normal_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 3)
	{
		meshList[GEO_BULLET]->textureID = Bullet_White;
		meshList[GEO_SMG]->textureID = Normal_SMG;
		RenderMesh(meshList[GEO_SMG], true);
	}
	else if (wepVal == 4)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Red;
		meshList[GEO_RIFLE]->textureID = Damage_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 5)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Red;
		meshList[GEO_SNIPER]->textureID = Damage_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 6)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Red;
		meshList[GEO_SMG]->textureID = Damage_SMG;
		RenderMesh(meshList[GEO_SMG], true);
	}
	else if (wepVal == 7)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Blue;
		meshList[GEO_RIFLE]->textureID = Capacity_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 8)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Blue;
		meshList[GEO_SNIPER]->textureID = Capacity_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 9)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Blue;
		meshList[GEO_SMG]->textureID = Capacity_SMG;
		RenderMesh(meshList[GEO_SMG], true);
	}
	else if (wepVal == 10)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Green;
		meshList[GEO_RIFLE]->textureID = Fast_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 11)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Green;
		meshList[GEO_SNIPER]->textureID = Fast_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 12)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Green;
		meshList[GEO_SMG]->textureID = Fast_SMG;
		RenderMesh(meshList[GEO_SMG], true);
	}
	else if (wepVal == 13)
	{
		//meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 1, 0), 8, 8);
		//meshList[GEO_SHOTGUN]->textureID = Fast_Shotgun;
		RenderMesh(meshList[GEO_CRYSTAL], true);
	}
	else if (wepVal == 14)
	{
		meshList[GEO_BULLET]->textureID = Bullet_Teal;
		//meshList[GEO_SHOTGUN]->textureID = Fast_Shotgun;
		//RenderMesh(meshList[GEO_CRYSTAL], true);
	}

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
}

/****************************************************************************/
/*!
\brief
Method that renders a mesh on screen
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Method that renders a mesh in the scene
*/
/****************************************************************************/

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

/****************************************************************************/
/*!
\brief
Initializes light settings
*/
/****************************************************************************/

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

	lightDefaultPos.Set(0, 40, 0);

	glUseProgram(m_programID);
	light[0].type = Light::LIGHT_DIRECTIONAL;
	light[0].position.Set(lightDefaultPos.x, lightDefaultPos.y, lightDefaultPos.z);
	light[0].color.Set(1, 0.95, 1);
	light[0].power = 0.7;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.0f;
	light[0].spotDirection.Set(0.0f, 1.0f, 0.0f);

	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	//Light 1 - Main Menu Pointer
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	glUseProgram(m_programID);
	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(UICrystalPosition.x, UICrystalPosition.y + 5, UICrystalPosition.z + 25);
	light[1].color.Set(0.3, 0.95, 1);
	light[1].power = 2.7;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.0f;
	light[1].spotDirection.Set(0.0f, 1.0f, 0.0f);

	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);
	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);
}

/****************************************************************************/
/*!
\brief
Method that renders the base in the scene
*/
/****************************************************************************/

void SP2_Scene::RenderBase()
{
	//RenderBase
	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_BASE], true);
	RenderMesh(meshList[GEO_CRYSTALBASE], true);
	RenderMesh(meshList[GEO_TELEPORTER], true);
	if (repairShipPhase == 0 || repairShipPhase == 1)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, tweenVal / 1000, 0);
		modelStack.Rotate(constRotation * 3, 0, 1, 0);
		RenderMesh(meshList[GEO_CRYSTAL], false);
		modelStack.PopMatrix();
	}
	modelStack.Scale(20, 1, 20);
	RenderMesh(meshList[GEO_MOONFLOOR], true);
	modelStack.PopMatrix();
	//RB End
}

/****************************************************************************/
/*!
\brief
Method that renders the Robots in the scene
*/
/****************************************************************************/

void SP2_Scene::RenderRobots()
{
	for (auto i : RobotManager.RobotList)
	{

		modelStack.PushMatrix();
		modelStack.Translate(i.Position().x, i.Position().y + 0.5, i.Position().z);
		modelStack.Rotate(i.Rotation, 0, 1, 0);
		//HP
		modelStack.PushMatrix();
		modelStack.Translate(0, 7, 0);
		modelStack.Rotate(constRotation * 2, 0, 1, 0);
		modelStack.PushMatrix();
		if (i.GetHealth() > 20)
		{
			modelStack.Scale(i.GetHealth() / 100, i.GetHealth() / 100, i.GetHealth() / 100);
			RenderMesh(meshList[GEO_ROBOTHEALTH], false);
		}
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Scale(i.GetHealth() / 100, i.GetHealth() / 100, i.GetHealth() / 100);
		if (i.GetHealth() >= 0)
		{
			RenderMesh(meshList[GEO_ROBOTHEALTH2], false);
		}
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Rotate(i.collapse, 1, 0, 0);
		RenderMesh(meshList[GEO_ROBOTBODY], true);
		modelStack.PushMatrix();
		modelStack.Rotate(i.rotatelefthand, 1, 0, 0);
		modelStack.Translate(0, 0, -10);
		modelStack.Translate(0, 0, 10);
		modelStack.Translate(0.3, 0, 0);
		RenderMesh(meshList[GEO_ROBOTLEFTUPPERARM], true);
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, i.leftarmattack);
		modelStack.Rotate(i.rotatelefthand, 1, 0, 0);
		RenderMesh(meshList[GEO_ROBOTLEFTLOWERARM], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Rotate(i.rotaterighthand, 1, 0, 0);
		modelStack.Translate(0, 0, -10);
		modelStack.Translate(0, 0, 10);
		modelStack.Translate(-0.3, 0, 0);
		RenderMesh(meshList[GEO_ROBOTRIGHTUPPERARM], true);
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, i.rightarmattack);
		modelStack.Rotate(i.rotaterighthand, 1, 0, 0);
		RenderMesh(meshList[GEO_ROBOTRIGHTLOWERARM], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Rotate(i.moveleftleg, 1, 0, 0);
		modelStack.Translate(0, 0, -10);
		modelStack.Translate(0, 0, 10);
		RenderMesh(meshList[GEO_ROBOTLEFTLEG], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Rotate(i.moverightleg, 1, 0, 0);
		RenderMesh(meshList[GEO_ROBOTRIGHTLEG], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PopMatrix();
	}
}

/****************************************************************************/
/*!
\brief
Method that controls the transition from wave to wave
*/
/****************************************************************************/

void SP2_Scene::GameState()
{
	if (basehp == 0 || playerhp == 0)
	{
		if (wave != 7)
		{
			WaveBGM->stop();
		}
		else WaveBGM2->stop();
		GameOver = true;
		wave = 0;
	}
	else if (wave == 1 && !weaponinterface && !SpawnedRobots)
	{
		PowerUpActive = true;
		PowerUpVal = rand() % 3 + 1;
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(0, 0, Vector3(spawnPointN.x + (rand() % 80 - 39), spawnPointN.y, spawnPointN.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 2 && !weaponinterface && !SpawnedRobots)
	{
		PowerUpActive = true;
		PowerUpVal = rand() % 3 + 1;
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(1, 180, Vector3(spawnPointS.x + (rand() % 80 - 39), spawnPointS.y, spawnPointS.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 3 && !weaponinterface && !SpawnedRobots)
	{
		PowerUpActive = true;
		PowerUpVal = rand() % 3 + 1;
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, -90, Vector3(spawnPointE.x + (rand() % 80 - 39), spawnPointE.y, spawnPointE.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 4 && !weaponinterface && !SpawnedRobots)
	{
		PowerUpActive = true;
		PowerUpVal = rand() % 3 + 1;
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, 90, Vector3(spawnPointW.x + (rand() % 80 - 39), spawnPointW.y, spawnPointW.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 5 && !weaponinterface && !SpawnedRobots)
	{
		PowerUpActive = true;
		PowerUpVal = rand() % 3 + 1;
		for (int i = 0; i < 3; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, 0, Vector3(spawnPointN.x + (rand() % 80 - 39), spawnPointN.y, spawnPointN.z + (rand() % 80 - 39))));
		}
		for (int i = 0; i < 3; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, 180, Vector3(spawnPointS.x + (rand() % 80 - 39), spawnPointS.y, spawnPointS.z + (rand() % 80 - 39))));
		}
		for (int i = 0; i < 3; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, -90, Vector3(spawnPointE.x + (rand() % 80 - 39), spawnPointE.y, spawnPointE.z + (rand() % 80 - 39))));
		}
		for (int i = 0; i < 3; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, 90, Vector3(spawnPointW.x + (rand() % 80 - 39), spawnPointW.y, spawnPointW.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	if (SpawnedRobots && RobotManager.CurrentRobotCount <= 0 && wave != 6 && wave != 7)
	{
		weaponinterface = true;
	}
	else
	{
		weaponinterface = false;
	}
	if (wave == 6 && repairShipPhase != 2 && repairShipPhase != 3 && repairShipPhase != 4)
	{
		repairShipPhase = 1;
	}
	if (wave >= 1 && wave <= 6)
	{
		RenderRocks();
		RenderRobots();
		RenderBase();
		RenderShip();
		RenderPowerUp(PowerUpVal);
		camera.spaceModeOn = false;
		if (!PlayBGM)
		{
			WaveBGM = engine->play2D(SoundName[1].c_str(), true, false, true);
			PlayBGM = true;
			beforeWave7 = true;
		}
		WaveBGM->setVolume(0.3 + VolumeIncrement);
	}
	else if (wave == 7)
	{
		if (ShipSpawned == false)
		{
			if (beforeWave7){ WaveBGM->stop(); PlayBGM = false; beforeWave7 = false; }
			if (!PlayBGM)
			{
				WaveBGM2 = engine->play2D(SoundName[2].c_str(), true, false, true);
				PlayBGM = true;
			}
			WaveBGM2->setVolume(0.3 + VolumeIncrement);
			weaponValue = 14;
			camera.Init(Vector3(0, 5, -30), Vector3(0, 5, 0), Vector3(0, 1, 0), 20);
			MothershipHandler.RobotList.push_back(Robot(3, 0, Vector3(300, 10, 700)));
			MothershipHandler.RobotList.front().isMothership = true;
			Intro = true;
			ShipSpawned = true;
			camera.maxCameraXrotation = 0;
			camera.minCameraXrotation = -10;
			camera.maxCameraYrotation = 5;
			camera.minCameraYrotation = -5;
			camera.spaceModeOn = true;
			MothershipHandler.RobotList.front().SetShipTarget(ShipPos1);
			WepSys.SetStats(weaponValue);
			CurrentAmmo = WepSys.MaxAmmo;
		}
		RenderSpaceMap();
	}
}

/****************************************************************************/
/*!
\brief
Method that renders Power Ups in the scene
*/
/****************************************************************************/

void SP2_Scene::RenderPowerUp(unsigned short PowerUpType)
{
	if (PowerUpType == 1)
	{
		meshList[GEO_POWERUP]->textureID = Bullet_Red;
		if (PowerUpType == 1 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 1 || weaponValue == 4 || weaponValue == 7 || weaponValue == 10))
		{
			weaponValue = 4;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			PowerUpActive = false;
			CurrentAmmo = WepSys.MaxAmmo;
		}
		else if (PowerUpType == 1 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 2 || weaponValue == 5 || weaponValue == 8 || weaponValue == 11))
		{
			weaponValue = 5;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			PowerUpActive = false;
			CurrentAmmo = WepSys.MaxAmmo;
		}
		else if (PowerUpType == 1 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 0 || weaponValue == 3 || weaponValue == 6 || weaponValue == 9 || weaponValue == 12))
		{
			weaponValue = 6;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			PowerUpActive = false;
			CurrentAmmo = WepSys.MaxAmmo;
		}
	}

	else if (PowerUpType == 2)
	{
		meshList[GEO_POWERUP]->textureID = Bullet_Blue;
		if (PowerUpType == 2 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 1 || weaponValue == 4 || weaponValue == 7 || weaponValue == 10))
		{
			weaponValue = 7;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			PowerUpActive = false;
			CurrentAmmo = WepSys.MaxAmmo;
		}
		else if (PowerUpType == 2 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 2 || weaponValue == 5 || weaponValue == 8 || weaponValue == 11))
		{
			weaponValue = 8;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			PowerUpActive = false;
			CurrentAmmo = WepSys.MaxAmmo;
		}
		else if (PowerUpType == 2 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 0 || weaponValue == 3 || weaponValue == 6 || weaponValue == 9 || weaponValue == 12))
		{
			weaponValue = 9;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			PowerUpActive = false;
			CurrentAmmo = WepSys.MaxAmmo;
		}
	}
	
	else if(PowerUpType == 3)
	{
		meshList[GEO_POWERUP]->textureID = Bullet_Green;
		if (PowerUpType == 3 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 1 || weaponValue == 4 || weaponValue == 7 || weaponValue == 10))
		{
			weaponValue = 10;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			CurrentAmmo = WepSys.MaxAmmo;
			PowerUpActive = false;
		}
		else if (PowerUpType == 3 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 2 || weaponValue == 5 || weaponValue == 8 || weaponValue == 11))
		{
			weaponValue = 11;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			CurrentAmmo = WepSys.MaxAmmo;
			PowerUpActive = false;
		}
		else if (PowerUpType == 3 && !camera.PowerUp.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y) && (weaponValue == 0 || weaponValue == 3 || weaponValue == 6 || weaponValue == 9 || weaponValue == 12))
		{
			weaponValue = 12;
			WepSys.ClearList();
			WepSys.SetStats(weaponValue);
			CurrentAmmo = WepSys.MaxAmmo;
			PowerUpActive = false;
		}
	}
	if (PowerUpActive == true)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, 14, 0);
		modelStack.Rotate(constRotation * 4, 0, 1, 0);
		modelStack.PushMatrix();
		modelStack.Scale(0.5, 0.5, 0.5);
			modelStack.PushMatrix();
			modelStack.Rotate(constRotation * 4, 1, 1, 0);
				RenderMesh(meshList[GEO_TORUS], false);
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Rotate(constRotation * 4, -1, 0, 1);
				RenderMesh(meshList[GEO_TORUS], false);
			modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Scale(0.3, 0.3, 0.3);
		RenderMesh(meshList[GEO_POWERUP], false);
		modelStack.PopMatrix();
	}
}

/****************************************************************************/
/*!
\brief
Method that renders the space scene
*/
/****************************************************************************/

void SP2_Scene::RenderSpaceMap()
{
	for (auto i : MothershipHandler.RobotList)
	{
		modelStack.PushMatrix();
		modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
		modelStack.Rotate(ShipRotation, 0, 1, 0);
		
		modelStack.PushMatrix();
		modelStack.Scale(20, 20, 20);
		modelStack.Translate(-i.explosion / 3, i.explosion / 2, 0);
		RenderMesh(meshList[GEO_MOTHERSHIP_TOP], true);
		modelStack.PushMatrix();
		modelStack.Translate(-i.explosion / 3, 0, i.explosion / 2);
		RenderMesh(meshList[GEO_MOTHERSHIP_UPPER], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(i.explosion / 2, i.explosion / 2, 0);
		RenderMesh(meshList[GEO_MOTHERSHIP_LOWER], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(i.explosion / 2, -i.explosion / 2, 0);
		RenderMesh(meshList[GEO_MOTHERSHIP_BOTTOM], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-i.explosion / 2, -i.explosion / 3, 0);
		RenderMesh(meshList[GEO_MOTHERSHIP_TAIL], true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 20, 0);
		modelStack.Scale(i.explosion * 10 + 10, i.explosion * 10 + 10, i.explosion * 10 + 10);
		RenderMesh(meshList[GEO_ENEMYBULLET], false);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
	}
	modelStack.PushMatrix();
	modelStack.Translate(PlayerShipCoord.x, PlayerShipCoord.y, PlayerShipCoord.z);
	modelStack.Rotate(90, 0, -1, 0);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GEO_PLAYERSHIP], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 30, 700);
	int range = 3;
	for (int detailLevel = 1; detailLevel <= range; detailLevel++)
	{
		for (int j = -6; j < 6; j += 12)
		{
			for (int i = 1; i <= 360; i += 60 - range * 5)
			{
				modelStack.PushMatrix();
				modelStack.Translate((detailLevel * 70 + 150) * cos(Math::DegreeToRadian(i + detailLevel * 20)), 40 * j, (detailLevel * 70 + 150) * sin(Math::DegreeToRadian(i + detailLevel * 20)));
				modelStack.Rotate(5 * i, 0, 1, 0);

				modelStack.PushMatrix();
				modelStack.Rotate(20 + 5 * (i + i * detailLevel), 1, 1, 1);
				modelStack.Scale(10, 15, 10);
				modelStack.Translate(0, moveRocks/5, 0);
				RenderMesh(meshList[GEO_METEOR], true);
				modelStack.PopMatrix();

				modelStack.PopMatrix();
			}
		}
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 90, 700);
	for (int detailLevel = 1; detailLevel <= range; detailLevel++)
	{
		for (int j = -6; j < 6; j += 12)
		{
			for (int i = 1; i <= 360; i += 60 - range * 5)
			{
				modelStack.PushMatrix();
				modelStack.Translate((detailLevel * 70 + 150) * cos(Math::DegreeToRadian(i + detailLevel * 20)), 40 * j, (detailLevel * 70 + 150) * sin(Math::DegreeToRadian(i + detailLevel * 20)));
				modelStack.Rotate(5 * i, 0, 1, 0);

				modelStack.PushMatrix();
				modelStack.Rotate(20 + 5 * (i + i * detailLevel), 1, 1, 1);
				modelStack.Scale(10, 15, 10);
				modelStack.Translate(0, moveRocks / 5, 0);
				RenderMesh(meshList[GEO_METEOR], true);
				modelStack.PopMatrix();

				modelStack.PopMatrix();
			}
		}
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 350, 700);
	for (int detailLevel = 1; detailLevel <= range; detailLevel++)
	{
		for (int j = -6; j < 6; j += 12)
		{
			for (int i = 1; i <= 360; i += 60 - range * 5)
			{
				modelStack.PushMatrix();
				modelStack.Translate((detailLevel * 70 + 150) * cos(Math::DegreeToRadian(i + detailLevel * 20)), 40 * j, (detailLevel * 70 + 150) * sin(Math::DegreeToRadian(i + detailLevel * 20)));
				modelStack.Rotate(5 * i, 0, 1, 0);

				modelStack.PushMatrix();
				modelStack.Rotate(20 + 5 * (i + i * detailLevel), 1, 1, 1);
				modelStack.Scale(10, 15, 10);
				modelStack.Translate(0, moveRocks / 5, 0);
				RenderMesh(meshList[GEO_METEOR], true);
				modelStack.PopMatrix();

				modelStack.PopMatrix();
			}
		}
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 300, 700);
	for (int detailLevel = 1; detailLevel <= range; detailLevel++)
	{
		for (int j = -6; j < 6; j += 12)
		{
			for (int i = 1; i <= 360; i += 60 - range * 5)
			{
				modelStack.PushMatrix();
				modelStack.Translate((detailLevel * 70 + 150) * cos(Math::DegreeToRadian(i + detailLevel * 20)), 40 * j, (detailLevel * 70 + 150) * sin(Math::DegreeToRadian(i + detailLevel * 20)));
				modelStack.Rotate(5 * i, 0, 1, 0);

				modelStack.PushMatrix();
				modelStack.Rotate(20 + 5 * (i + i * detailLevel), 1, 1, 1);
				modelStack.Scale(10, 15, 10);
				modelStack.Translate(0, moveRocks / 5, 0);
				RenderMesh(meshList[GEO_METEOR], true);
				modelStack.PopMatrix();

				modelStack.PopMatrix();
			}
		}
	}
	modelStack.PopMatrix();

	for (auto i : EnemyWepSys.BulletList)
	{
		modelStack.PushMatrix();
		modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_ENEMYBULLET], false);
		modelStack.PopMatrix();
	}
}

/****************************************************************************/
/*!
\brief
Method that renders and animates the drivable ship
*/
/****************************************************************************/

void SP2_Scene::RenderShip()
{
	if (repairShipPhase == 1 || repairShipPhase == 2)
	{
		modelStack.PushMatrix(); //Player ship
		modelStack.Translate(ShipX, ShipY, 75);
		modelStack.Rotate(45, 1, 0, 1.75);
		modelStack.Scale(4, 4, 4);
		RenderMesh(meshList[GEO_PLAYERSHIP], true);
		modelStack.PopMatrix();
	}
	if (camera.shipCheck == true && ShipY <= 1 && repairShipPhase == 1)
	{
		RenderImageOnScreen(UI_BG, 95, 2.75, 82.5, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Bring the base core to the ship in order to refuel it.", Color(0.000f, 0.808f, 0.820f), 3, 40, 25);
	}
	if (camera.coreCheck == true && repairShipPhase == 1)
	{
		RenderImageOnScreen(UI_BG, 37.5, 2.75, 77.5, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <E> to pick up Core", Color(0.000f, 0.808f, 0.820f), 2.45, 62.5, 25);
	}
	if (camera.shipCheck == true && repairShipPhase == 2)
	{
		RenderImageOnScreen(UI_BG, 37.5, 2.75, 77.5, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <E> to insert Core", Color(0.000f, 0.808f, 0.820f), 2.45, 62.5, 25);
	}

	if (repairShipPhase == 3)
	{
		modelStack.PushMatrix();
		modelStack.Translate(ShipX, ShipY, 75);
		modelStack.Scale(4, 4, 4);
		RenderMesh(meshList[GEO_PLAYERSHIP], true);
		modelStack.PopMatrix();

		if (camera.shipCheck == true)
		{
			RenderImageOnScreen(UI_BG, 37.5, 2.75, 77.5, 25);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press <F> to pilot ship", Color(0.000f, 0.808f, 0.820f), 2.45, 62.5, 25);
		}
	}

	if (repairShipPhase == 4)
	{
		modelStack.PushMatrix();
		modelStack.Translate(ShipX, ShipY, 75);
		modelStack.Rotate(ShipRot, 0, 0, 1);
		modelStack.Scale(4, 4, 4);
		RenderMesh(meshList[GEO_PLAYERSHIP], true);
		modelStack.PopMatrix();
		camera.Init(Vector3(0, 15, 250), Vector3(0, 5, 0), Vector3(0, 1, 0), 20);
		camera.CameraYrotation = 180;
		weaponValue = 15;
	}
}

/****************************************************************************/
/*!
\brief
Method that pushes the bullet patterns of the enemy mothership
*/
/****************************************************************************/

void SP2_Scene::MothershipBulletSpawner(float BulletDamage, float BulletSpeed, int choice, Vector3 position)
{
	if (choice == 1){
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + PlayerShipCoord).Normalize()));
		ShotTimer = 0;
	}
	else if (choice == 2){
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(0, 5, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(5, 0, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(-5, 0, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(0, -5, 0) + PlayerShipCoord).Normalize()));
		ShotTimer = 0;
	}
	else if (choice == 3){
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(0, 5, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(0, -5, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(0, 0, 0) + PlayerShipCoord).Normalize()));
		ShotTimer = 0;
	}
	else if (choice == 4){
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(5, 0, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(-5, 0, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(0, 0, 0) + PlayerShipCoord).Normalize()));
		ShotTimer = 0;
	}
	else if (choice == 5){
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(5, 5, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(5, -5, 0) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(-5, 5, -15) + PlayerShipCoord).Normalize()));
		EnemyWepSys.BulletList.push_back(RayCast(BulletDamage, BulletSpeed, position, (-position + Vector3(-5, -5, -15) + PlayerShipCoord).Normalize()));
		ShotTimer = 0;
	}
}

/****************************************************************************/
/*!
\brief
Method where values are changed
*/
/****************************************************************************/

void SP2_Scene::Update(double dt)
{
	camera.Update(dt);
	constRotation += (float)(10 * dt);
	constTranslation += (float)(10 * dt);
	//RobotAnimation(dt);
	//Lerping Rotation
	if ((rLimiter == true))
	{
		tweenVal += (float)(50 * dt);
	}
	else if ((rLimiter == false))
	{
		tweenVal -= (float)(50 * dt);
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
	translateX += (float)(10 * pause  * dt);

	//mothership
	if (moveMship == true)
	{
		moveMshipUp += (float)(1 * dt);
	}
	else if (moveMship == false)
	{
		moveMshipUp -= (float)(1 * dt);
	}
	if (moveMshipUp > 6)
	{
		moveMship = false;
	}
	else if (moveMshipUp < -3)
	{
		moveMship = true;
	}
	rotateMship += (float)(2 * dt);

	if (floatRocks == true)
	{
		moveRocks += (float)(1 * dt);
	}
	else if (floatRocks == false)
	{
		moveRocks -= (float)(1 * dt);
	}
	if (moveRocks > 35)
	{
		floatRocks = false;
	}
	else if (moveRocks < -12)
	{
		floatRocks = true;
	}

	if (GameLoading)
	{
		LoadTimer += dt;
		if (LoadTimer >= MaxLoadTime * 2 / 6 && !loadMap)
		{
			InitMapModels();
			loadMap = true;
		}
		else if (LoadTimer >= MaxLoadTime * 4 / 6 && !loadWep)
		{
			InitKeyVariables();
			InitWeaponModels();
			loadWep = true;
		}
		else if (LoadTimer >= MaxLoadTime * 5 / 6 && !loadRobots)
		{
			InitRobots();
			loadRobots = true;
		}
		if (LoadTimer > MaxLoadTime)
		{
			GameLoading = false;
			InMainMenu = true;
			buttonValue = 0;
		}
	}
	else if (InMainMenu)
	{
		if (!PlayBGM)
		{
			MenuBGM = engine->play2D(SoundName[3].c_str(), true, false, true);
			PlayBGM = true;
		}
		MenuBGM->setVolume(0.3 + VolumeIncrement);
		light[1].color.Set(0.3, 0.95, 1);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		camera.CanMoveCamera = false;
		camera.maxCameraXrotation = 5;
		camera.minCameraXrotation = -5;
		camera.maxCameraYrotation = 5;
		camera.minCameraYrotation = -5;
		UICrystalPosition.y = -5 - camera.CameraXrotation;
		if (!InSettings)
		{
			if (camera.CameraXrotation >= 2.5 && camera.CameraXrotation <= 5)
			{
				UICrystalChoice = 4;
			}
			else if (camera.CameraXrotation >= 0 && camera.CameraXrotation <= 2.5)
			{
				UICrystalChoice = 3;
			}
			else if (camera.CameraXrotation >= -2.5 && camera.CameraXrotation <= 0)
			{
				UICrystalChoice = 2;
			}
			else if (camera.CameraXrotation >= -5 && camera.CameraXrotation <= -2.5)
			{
				UICrystalChoice = 1;
			}
			else UICrystalChoice2 = 0;
		}
		else if (InSettings)
		{
			if (camera.CameraXrotation >= 3 && camera.CameraXrotation <= 5)
			{
				UICrystalChoice2 = 3;
			}
			else if (camera.CameraXrotation >= -1 && camera.CameraXrotation <= 3)
			{
				UICrystalChoice2 = 2;
			}
			else if (camera.CameraXrotation >= -4 && camera.CameraXrotation <= -1)
			{
				UICrystalChoice2 = 1;
			}
			else UICrystalChoice2 = 0;
		}

		if (!InSettings && !InstructionScreen && buttonPress && UICrystalChoice == 1 && InMainMenu && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
			buttonPress = false;
			buttonValue = 0;
			InMainMenu = false;
			GameStarted = true;
			camera.CanMoveCamera = true;
			MenuBGM->stop();
			PlayBGM = false;
			InitKeyVariables();
			WepSys.SetStats(weaponValue);
			CurrentAmmo = WepSys.MaxAmmo;
		}
		else if (!InSettings && !InstructionScreen &&buttonPress && UICrystalChoice == 2 && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			buttonPress = false;
			buttonValue = 0;
			InstructionScreen = true;
			InstrScrnMove = 0;
		}
		else if (!InSettings && !InstructionScreen &&buttonPress && UICrystalChoice == 3 && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			buttonPress = false;
			buttonValue = 0;
			InSettings = true;
		}
		else if (!InSettings && !InstructionScreen &&buttonPress && UICrystalChoice == 4 && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			buttonPress = false;
			buttonValue = 0;
			MenuBGM->stop();
			PlayBGM = false;
			Application::CloseGame = true;
		}
		if (buttonPress && Application::IsKeyPressed(VK_RBUTTON))
		{
			buttonPress = false;
			buttonValue = 0;
			InstructionScreen = false;
			InSettings = false;
			InstrScrnMove = 0;
		}
		if (!InstructionScreen && !InSettings && UIPushBackVal > 0)
		{
			UIPushBackVal -= (float)(15 * dt);
		}
		else if ((InstructionScreen || InSettings) && UIPushBackVal <= 20)
		{
			UIPushBackVal += (float)(15 * dt);
		}
		if (InstructionScreen && (InstrScrnMove + camera.CameraYrotation * 2 * dt) <= 0 && (InstrScrnMove + camera.CameraYrotation * 2 * dt) >= -90 && (camera.CameraYrotation < -1.5 || camera.CameraYrotation > 1.5))
		{
			InstrScrnMove += (float)(camera.CameraYrotation * 2 * dt);
		}
		if (InSettings && !InstructionScreen && buttonPress && UICrystalChoice2 == 1 && Application::IsKeyPressed(VK_LBUTTON))
		{
			if (camera.MouseSensitivity == 1)
			{
				camera.MouseSensitivity = 2;
			}
			else if (camera.MouseSensitivity == 2)
			{
				camera.MouseSensitivity = 0.5;
			}
			else if (camera.MouseSensitivity == 0.5)
			{
				camera.MouseSensitivity = 1;
			}
			buttonValue = 0;
			buttonPress = false;
		}
		else if (InSettings && !InstructionScreen && buttonPress && UICrystalChoice2 == 2 && Application::IsKeyPressed(VK_LBUTTON))
		{
			if (TestingModeON)
			{
				TestingModeON = false;
			}
			else if (!TestingModeON)
			{
				TestingModeON = true;
			}
			buttonValue = 0;
			buttonPress = false;
		}
		else if (InSettings && !InstructionScreen && buttonPress && UICrystalChoice2 == 3 && Application::IsKeyPressed(VK_LBUTTON))
		{
			if (VolumeLevel == 1)
			{
				VolumeLevel = 2;
				VolumeIncrement = 0.2;
			}
			else if (VolumeLevel == 2)
			{
				VolumeLevel = 0;
				VolumeIncrement = -0.2;
			}
			else if (VolumeLevel == 0)
			{
				VolumeLevel = 1;
				VolumeIncrement = 0;
			}
			buttonValue = 0;
			buttonPress = false;
		}
		if (!buttonPress)
		{
			buttonValue += dt;
			if (buttonValue >= 1)
			{
				buttonPress = true;
			}
		}
	}
	else if (GameOver)
	{
		if (Application::IsKeyPressed(VK_RBUTTON))
		{
			InitKeyVariables();
			basehp = 100;
			playerhp = 100;
			wave = 1;
			ShipSpawned = false;
			GameOver = false;
			SpawnedRobots = false;
			camera.spaceModeOn = false;
			camera.Init(Vector3(0, 5, 3), Vector3(0, 5, 0), Vector3(0, 1, 0), 20);
			camera.baseWalkSpeed = 4;
			camera.Reset();
			repairShipPhase = 0;
			PlayBGM = false;
			IsReloading = true;
			weaponValue = 0;
			InMainMenu = true;
		}
	}
	else if (GameStarted)
	{
		//Resetting Scaling
		if (scaleAll >= scalingLimit)
		{
			scaleAll = 5;
		}
		scaleAll += (float)(2 * pause  * dt);
		if (TestingModeON && Application::IsKeyPressed('6'))
		{
			glEnable(GL_CULL_FACE);
		}
		if (TestingModeON &&Application::IsKeyPressed('7'))
		{
			glDisable(GL_CULL_FACE);
		}
		if (TestingModeON &&Application::IsKeyPressed('8'))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (TestingModeON && Application::IsKeyPressed('9'))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (TestingModeON && buttonPress && Application::IsKeyPressed(VK_BACK))
		{
			RobotManager.ClearList();
			EnemyWepSys.ClearList();
		}
		if (TestingModeON && buttonPress && Application::IsKeyPressed(VK_OEM_PLUS))
		{
			playerhp = 100;
		}
		if (TestingModeON && buttonPress && Application::IsKeyPressed(VK_OEM_MINUS))
		{
			CurrentAmmo = WepSys.MaxAmmo;
		}

		//Weapon
		if (weaponinterface && randWepChoices)
		{
			srand(constTranslation);
			WepItf_Choices.x = rand() % 4;
			WepItf_Choices.y = rand() % 4;
			WepItf_Choices.z = rand() % 4;
			randWepChoices = false;
		}
		WepSys.SetStats(weaponValue);
		if (!buttonPress)
		{
			buttonValue += dt;
			if (buttonValue >= 2)
			{
				buttonPress = true;
			}
		}
		if (weaponinterface == true)
		{
			if (buttonPress == true && Application::IsKeyPressed('1'))
			{
				if (WepItf_Choices.x == 0){ weaponValue = 1; }
				else if (WepItf_Choices.x == 1){ weaponValue = 4; }
				else if (WepItf_Choices.x == 2){ weaponValue = 7; }
				else if (WepItf_Choices.x == 3){ weaponValue = 10; }
				WepSys.ClearList();
				wave += 1;
				buttonPress = false;
				buttonValue = 0;
				weaponinterface = false;
				WepSys.SetStats(weaponValue);
				CurrentAmmo = WepSys.MaxAmmo;
				SpawnedRobots = false;
			}
			else if (buttonPress == true && Application::IsKeyPressed('2'))
			{
				if (WepItf_Choices.y == 0){ weaponValue = 2; }
				else if (WepItf_Choices.y == 1){ weaponValue = 5; }
				else if (WepItf_Choices.y == 2){ weaponValue = 8; }
				else if (WepItf_Choices.y == 3){ weaponValue = 11; }
				WepSys.ClearList();
				wave += 1;
				buttonPress = false;
				buttonValue = 0;
				weaponinterface = false;
				WepSys.SetStats(weaponValue);
				CurrentAmmo = WepSys.MaxAmmo;
				SpawnedRobots = false;
			}
			else if (buttonPress == true && Application::IsKeyPressed('3'))
			{
				if (WepItf_Choices.z == 0){ weaponValue = 3; }
				else if (WepItf_Choices.z == 1){ weaponValue = 6; }
				else if (WepItf_Choices.z == 2){ weaponValue = 9; }
				else if (WepItf_Choices.z == 3){ weaponValue = 12; }
				WepSys.ClearList();
				wave += 1;
				buttonPress = false;
				buttonValue = 0;
				weaponinterface = false;
				WepSys.SetStats(weaponValue);
				CurrentAmmo = WepSys.MaxAmmo;
				SpawnedRobots = false;
			}
			else if (buttonPress == true && Application::IsKeyPressed('4'))
			{
				WepSys.ClearList();
				wave += 1;
				buttonPress = false;
				buttonValue = 0;
				weaponinterface = false;
				WepSys.SetStats(weaponValue);
				CurrentAmmo = WepSys.MaxAmmo;
				SpawnedRobots = false;
			}
		}
		if (!weaponinterface)
		{
			randWepChoices = true;
		}

		if (ShipY >= 1 && (repairShipPhase == 1 || repairShipPhase == 2))
		{
			ShipX += 0.1;
			ShipY -= 1;
		}
		if (repairShipPhase == 3)
		{
			ShipY = 1;
		}
		if (ShipX <= 20 && repairShipPhase == 4)
		{
			ShipX += 5 * dt;
		}
		if (ShipX >= 20 && ShipX <= 200 && repairShipPhase == 4)
		{
			ShipX += 15 * dt;
			ShipY += 10 * dt;
		}
		if (ShipX >= 20 && ShipX <= 200 && ShipRot <= 30 && repairShipPhase == 4)
		{
			ShipRot += 10 * dt;
		}
		if (ShipX >= 140 && repairShipPhase == 4)
		{
			repairShipPhase = 5;
			wave = 7;
		}
		if (camera.coreCheck == true && Application::IsKeyPressed('E') && repairShipPhase == 1)
		{
			repairShipPhase = 2;
			weaponValue = 13;
			CurrentAmmo = WepSys.MaxAmmo;
			IsReloading = true;
		}
		if (camera.shipCheck == true && Application::IsKeyPressed('E') && repairShipPhase == 2)
		{
			repairShipPhase = 3;
			weaponValue = 15;
			CurrentAmmo = WepSys.MaxAmmo;
			IsReloading = true;
		}
		if (camera.shipCheck == true && Application::IsKeyPressed('F') && repairShipPhase == 3)
		{
			repairShipPhase = 4;
			weaponValue = 15;
			CurrentAmmo = WepSys.MaxAmmo;
			IsReloading = true;
		}

		framesPerSecond = 1 / dt;

		TownLightPosition.y += tweenVal / 15000;
		light[1].position.Set(TownLightPosition.x, TownLightPosition.y, TownLightPosition.z);
		RoomLightPosition.y += tweenVal / 150000;
		light[2].position.Set(RoomLightPosition.x, RoomLightPosition.y, RoomLightPosition.z);

		if (!CanFire)
		{
			GunWaitTime += pause * dt;
			if (GunWaitTime >= WepSys.RateOfFire)
			{
				CanFire = true;
			}
		}
		if (IsReloading)
		{
			ReloadWaitTime += pause * dt;
			if (ReloadWaitTime >= WepSys.ReloadTime)
			{
				IsReloading = false;
				CurrentAmmo = WepSys.MaxAmmo;
			}
		}
		if (!IsReloading && CanFire && weaponValue != 13 && weaponValue < 15 && Application::IsKeyPressed(VK_LBUTTON))
		{
			if (CurrentAmmo <= 0)
			{
				IsReloading = true;
				ReloadWaitTime = 0;
			}
			else {
				CurrentAmmo -= 1;
				WepSys.BulletList.push_back(RayCast(WepSys.Damage, WepSys.Speed, camera.getCameraPosition(), camera.getLookVector()));
				ShootSound = engine->play2D(SoundName[0].c_str(), false, false, true);
				ShootSound->setVolume(0.6 + VolumeIncrement);
				CanFire = false;
				GunWaitTime = 0;
			}
		}
		
		if (Application::IsKeyPressed('R'))
		{
			IsReloading = true;
			ReloadWaitTime = 0;
		}
		if (Application::IsKeyPressed(VK_RBUTTON) && (weaponValue == 2 || weaponValue == 5 || weaponValue == 8 || weaponValue == 11))
		{
			Mtx44 projection;
			projection.SetToPerspective(20.0f, static_cast <float>(S_Width) / static_cast <float>(S_Height), 0.1f, 3000.0f);
			projectionStack.LoadMatrix(projection);
		}
		else {
			Mtx44 projection;
			projection.SetToPerspective(40.0f, static_cast <float>(S_Width) / static_cast <float>(S_Height), 0.1f, 3000.0f);
			projectionStack.LoadMatrix(projection);
		}

		WepSys.IncrementPosition();
		EnemyWepSys.IncrementPosition();
		RobotManager.IncrementPosition();
		MothershipHandler.IncrementPosition();
		PlayerShipCoord = Vector3(camera.getCameraPosition().x, camera.getCameraPosition().y - 2, camera.getCameraPosition().z + 10);
		PlayerShipBounds.set(PlayerShipCoord.x - 6, PlayerShipCoord.x + 6, PlayerShipCoord.z - 5, PlayerShipCoord.z + 5, PlayerShipCoord.y - 3, PlayerShipCoord.y + 3);
		Boundary Core(-2.5, 2.5, -2.5, 2.5, -5, 10);
		Boundary PlayerBounds;
		PlayerBounds.set(camera.getCameraPosition().x - 1.5, camera.getCameraPosition().x + 1.5, camera.getCameraPosition().z - 1.5, camera.getCameraPosition().z + 1.5, camera.getCameraPosition().y - 1.5, camera.getCameraPosition().y + 5);
		for (std::list<Robot>::iterator iter = RobotManager.RobotList.begin(); iter != RobotManager.RobotList.end(); ++iter)
		{
			(*iter).BoundsCheck(WepSys);
			(*iter).RobotAnimation(dt);
			if (!Core.BoundaryCheck((*iter).Position().x, (*iter).Position().z, (*iter).Position().y))
			{
				basehp = 0;
			}
			if (!(*iter).BoundingBox.BoundaryCheck(camera.getCameraPosition().x, camera.getCameraPosition().z, camera.getCameraPosition().y))
			{
				playerhp -= 1;
			}
			
		}
		if (ShipRotation >= 0)
		{
			ShipRotation -= 0.1;
		}
		else
		{
			weaponsOn = true; 
		}
		if (Intro == true)
		{
			Intro = false;
			MothershipHandler.RobotList.front().SetShipTarget(ShipPos1);
			ShipRotation = 90;
		}
		if (ShipSpawned)
		{
			for (std::list<Robot>::iterator iter = MothershipHandler.RobotList.begin(); iter != MothershipHandler.RobotList.end(); ++iter)
			{
				(*iter).RobotAnimation(dt);
			}
			if (weaponsOn && MothershipHandler.RobotList.front().GetHealth() >= 0)
			{
				for (std::list<Robot>::iterator iter = MothershipHandler.RobotList.begin(); iter != MothershipHandler.RobotList.end(); ++iter)
				{
					(*iter).BoundsCheck(WepSys);
				}
				srand(constTranslation);
				ChoiceTimer += dt;
				float HPRatio = MothershipHandler.RobotList.front().GetHealth() / 5000;
				if (ChoiceTimer > (5 * HPRatio))
				{
					int choice = rand() % 3 + 1;
					if (choice == 1){ MothershipHandler.RobotList.front().SetShipTarget(ShipPos1); }
					else if (choice == 2){ MothershipHandler.RobotList.front().SetShipTarget(ShipPos2); }
					else if (choice == 3){ MothershipHandler.RobotList.front().SetShipTarget(ShipPos3); }
					ChoiceTimer = 0;
				}
				ShotTimer += dt;
				if (ShotTimer > 1)
				{
					float BulletDamage = 2;
					float BulletSpeed = 5 / HPRatio;
					if (BulletSpeed > 15){ BulletSpeed = 15; }
					int choice = rand() % 5 + 1;
					Vector3 ShootPos1(MothershipHandler.RobotList.front().Position().x, MothershipHandler.RobotList.front().Position().y, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos2(MothershipHandler.RobotList.front().Position().x + 20, MothershipHandler.RobotList.front().Position().y, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos3(MothershipHandler.RobotList.front().Position().x - 20, MothershipHandler.RobotList.front().Position().y, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos4(MothershipHandler.RobotList.front().Position().x, MothershipHandler.RobotList.front().Position().y + 30, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos5(MothershipHandler.RobotList.front().Position().x + 20, MothershipHandler.RobotList.front().Position().y + 30, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos6(MothershipHandler.RobotList.front().Position().x - 20, MothershipHandler.RobotList.front().Position().y + 30, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos7(MothershipHandler.RobotList.front().Position().x, MothershipHandler.RobotList.front().Position().y - 20, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos8(MothershipHandler.RobotList.front().Position().x + 20, MothershipHandler.RobotList.front().Position().y - 20, MothershipHandler.RobotList.front().Position().z);
					Vector3 ShootPos9(MothershipHandler.RobotList.front().Position().x - 20, MothershipHandler.RobotList.front().Position().y - 20, MothershipHandler.RobotList.front().Position().z);

					if (MothershipHandler.RobotList.front().GetHealth() < 5000)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos1);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 4500)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos2);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 4000)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos3);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 3500)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos4);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 3000)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos5);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 2500)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos6);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 2000)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos7);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 1500)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos8);
					}
					if (MothershipHandler.RobotList.front().GetHealth() < 1000)
					{
						MothershipBulletSpawner(BulletDamage, BulletSpeed, choice, ShootPos9);
					}
				}
			}
			else if (MothershipHandler.RobotList.front().GetHealth() <= 0 && MothershipHandler.RobotList.front().deadanimationover == true)
			{
				GameOver = true;
			}
			if ((MothershipHandler.RobotList.front().GetHealth() <= 0 && MothershipHandler.RobotList.front().die == true && !playOnce))
			{
				playOnce = true;
				ExplosionSound = engine->play2D(SoundName[4].c_str(), false, false, true);
				ShootSound->setVolume(0.6 + VolumeIncrement);
				WaveBGM2->stop();
			}
		}
		
		for (std::list<RayCast>::iterator iter = EnemyWepSys.BulletList.begin(); iter != EnemyWepSys.BulletList.end(); ++iter)
		{
			if (!PlayerShipBounds.BoundaryCheck((*iter).Position().x, (*iter).Position().z, (*iter).Position().y))
			{
				playerhp -= (*iter).Damage;
			}
		}
		RobotManager.CleanUp();
		WepSys.CleanUp();
		EnemyWepSys.CleanUp();
		MothershipHandler.CleanUp();
	}
}

/****************************************************************************/
/*!
\brief
Method that renders the skybox
*/
/****************************************************************************/

void SP2_Scene::RenderSkybox(Vector3 Position)
{
	float scaleSB = 2500;
	float transSB = scaleSB / 2 - scaleSB / 1250;
	//Skybox
	modelStack.PushMatrix();
	//Skybox Movement
	modelStack.Translate(Position.x, Position.y, Position.z);
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

/****************************************************************************/
/*!
\brief
Method that renders the weapon select screen
*/
/****************************************************************************/

void SP2_Scene::RenderWepScreen(bool render, Vector3 choices)
{
	if (render){
		std::string Type1, Type2, Type3;
		if (choices.x == 0){ Type1 = "Type: <Normal>"; }
		else if (choices.x == 1){ Type1 = "Type: <Damage>"; }
		else if (choices.x == 2){ Type1 = "Type: <Capacity>"; }
		else if (choices.x == 3){ Type1 = "Type: <Speed>"; }
		//Wep Select UI - Overall
		modelStack.PushMatrix();
		RenderImageOnScreen(UI_WepSel_BG, 130, 65, 80, 45);
		//Left
		RenderImageOnScreen(UI_BG, 30, 30, 40, 45);
		if (choices.x == 0)
		{
			meshList[GEO_RIFLE]->textureID = Normal_Rifle;
			RenderMeshOnScreen(meshList[GEO_RIFLE], 1, 1, 40, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.x == 1)
		{
			meshList[GEO_RIFLE]->textureID = Damage_Rifle;
			RenderMeshOnScreen(meshList[GEO_RIFLE], 1, 1, 40, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.x == 2)
		{
			meshList[GEO_RIFLE]->textureID = Capacity_Rifle;
			RenderMeshOnScreen(meshList[GEO_RIFLE], 1, 1, 40, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.x == 3)
		{
			meshList[GEO_RIFLE]->textureID = Fast_Rifle;
			RenderMeshOnScreen(meshList[GEO_RIFLE], 1, 1, 40, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		RenderImageOnScreen(UI_BG, 30, 10, 40, 67.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Gun: Rifle", Color(0.000f, 0.808f, 0.820f), 4, 29, 70);
		RenderTextOnScreen(meshList[GEO_TEXT], Type1, Color(0.000f, 0.808f, 0.820f), 3, 29, 66);
		RenderImageOnScreen(UI_BG, 30, 2.75, 40, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <1> to select", Color(0.000f, 0.808f, 0.820f), 2.5, 28.5, 25);

		if (choices.y == 0){ Type2 = "Type: <Normal>"; }
		else if (choices.y == 1){ Type2 = "Type: <Damage>"; }
		else if (choices.y == 2){ Type2 = "Type: <Capacity>"; }
		else if (choices.y == 3){ Type2 = "Type: <Speed>"; }
		//Center
		RenderImageOnScreen(UI_BG, 30, 30, 80, 45);
		if (choices.y == 0)
		{
			meshList[GEO_SNIPER]->textureID = Normal_Sniper;
			RenderMeshOnScreen(meshList[GEO_SNIPER], 1, 1, 85, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.y == 1)
		{
			meshList[GEO_SNIPER]->textureID = Damage_Sniper;
			RenderMeshOnScreen(meshList[GEO_SNIPER], 1, 1, 85, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.y == 2)
		{
			meshList[GEO_SNIPER]->textureID = Capacity_Sniper;
			RenderMeshOnScreen(meshList[GEO_SNIPER], 1, 1, 85, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.y == 3)
		{
			meshList[GEO_SNIPER]->textureID = Fast_Sniper;
			RenderMeshOnScreen(meshList[GEO_SNIPER], 1, 1, 85, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		RenderImageOnScreen(UI_BG, 30, 10, 80, 67.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Gun: Sniper", Color(0.000f, 0.808f, 0.820f), 4, 69, 70);
		RenderTextOnScreen(meshList[GEO_TEXT], Type2, Color(0.000f, 0.808f, 0.820f), 3, 69, 66);
		RenderImageOnScreen(UI_BG, 30, 2.75, 80, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <2> to select", Color(0.000f, 0.808f, 0.820f), 2.5, 68.5, 25);

		if (choices.z == 0){ Type3 = "Type: <Normal>"; }
		else if (choices.z == 1){ Type3 = "Type: <Damage>"; }
		else if (choices.z == 2){ Type3 = "Type: <Capacity>"; }
		else if (choices.z == 3){ Type3 = "Type: <Speed>"; }
		//Right
		RenderImageOnScreen(UI_BG, 30, 30, 120, 45);
		if (choices.z == 0)
		{
			meshList[GEO_SMG]->textureID = Normal_SMG;
			RenderMeshOnScreen(meshList[GEO_SMG], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.z == 1)
		{
			meshList[GEO_SMG]->textureID = Damage_SMG;
			RenderMeshOnScreen(meshList[GEO_SMG], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.z == 2)
		{
			meshList[GEO_SMG]->textureID = Capacity_SMG;
			RenderMeshOnScreen(meshList[GEO_SMG], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.z == 3)
		{
			meshList[GEO_SMG]->textureID = Fast_SMG;
			RenderMeshOnScreen(meshList[GEO_SMG], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		RenderImageOnScreen(UI_BG, 30, 10, 120, 67.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Gun: SMG", Color(0.000f, 0.808f, 0.820f), 4, 109, 70);
		RenderTextOnScreen(meshList[GEO_TEXT], Type3, Color(0.000f, 0.808f, 0.820f), 3, 109, 66);
		RenderImageOnScreen(UI_BG, 30, 2.75, 120, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <3> to select", Color(0.000f, 0.808f, 0.820f), 2.5, 108.5, 25);
		RenderImageOnScreen(UI_BG, 50, 2.75, 80, 17.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <4> to keep current weapon", Color(0.000f, 0.808f, 0.820f), 2.5, 59.5, 17.5);
		modelStack.PopMatrix();
		//Wep Select UI END
	}
}

/****************************************************************************/
/*!
\brief
Method that renders the teleporters in the scene
*/
/****************************************************************************/

void SP2_Scene::RenderTeleporter(bool render)
{
	if (render)
	{
		modelStack.PushMatrix();
		RenderImageOnScreen(UI_BG, 30, 2.75, 80, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <E> to activate", Color(0.000f, 0.808f, 0.820f), 2.45, 67.5, 25);
		modelStack.PopMatrix();
	}
}

/****************************************************************************/
/*!
\brief
Method that renders the user interface
*/
/****************************************************************************/

void SP2_Scene::RenderGameUI()
{
	//Render In-Hand Weapon
	modelStack.PushMatrix();
	RenderWeaponInHand(weaponValue, 5, 1, 1);
	modelStack.PopMatrix();

	//INFO UI, STATS - BOTTOM LEFT
	modelStack.PushMatrix();
	RenderImageOnScreen(UI_BG, 50, 10, 25, 5);
	std::stringstream fpsText;
	fpsText << std::fixed << std::setprecision(2) << "FPS = " << framesPerSecond;
	RenderTextOnScreen(meshList[GEO_TEXT], fpsText.str(), Color(1, 1, 1), 2, 3, 7.5);
	std::stringstream coordText;
	coordText << std::fixed << std::setprecision(1) << "Player Location = (" << camera.getCameraPosition().x << ", " << camera.getCameraPosition().y << ", " << camera.getCameraPosition().z << ")";
	RenderTextOnScreen(meshList[GEO_TEXT], coordText.str(), Color(1, 1, 1), 2, 3, 4);
	modelStack.PopMatrix();
	//INFO UI, STATS END

	//INFO UI, HP - CENTER
	modelStack.PushMatrix();
	RenderImageOnScreen(UI_BG, 50, 10, 80, 5);
	RenderTextOnScreen(meshList[GEO_TEXT], "<Player Health>", Color(0, 1, 0), 3, 70, 7);
	RenderImageOnScreen(UI_HP_Red, 40, 3, 80, 3);
	float Dividend = playerhp * 0.4;
	if (playerhp > 0){ RenderImageOnScreen(UI_HP_Green, Dividend, 3, 80, 3); }
	modelStack.PopMatrix();
	//INFO UI, HP END

	//INFO UI, Enemy HP - TOP
	if (ShipSpawned && MothershipHandler.RobotList.front().GetHealth() >= 0)
	{
		RenderImageOnScreen(UI_BG, 50, 10, 80, 80);
		RenderTextOnScreen(meshList[GEO_TEXT], "<Mothership Health>", Color(0, 1, 0), 3, 66, 82);
		RenderImageOnScreen(UI_HP_Red, 40, 3, 80, 78);
		float Dividend = MothershipHandler.RobotList.front().GetHealth() / 5000 * 40;
		if (playerhp > 0){ RenderImageOnScreen(UI_HP_Green, Dividend, 3, 80, 78); }
	}
	//INFO UI, Enemy HP END

	//INFO UI, WEP STATS - BOTTOM RIGHT
	modelStack.PushMatrix();
	RenderImageOnScreen(UI_BG, 50, 10, 135, 5);
	std::stringstream ammoText;
	if (weaponValue != 13 && weaponValue < 15)
	{
		ammoText << std::fixed << std::setprecision(0) << "Ammo - < " << CurrentAmmo << " / " << WepSys.MaxAmmo << " >";
	}
	else ammoText << std::fixed << std::setprecision(0) << "<No Weapon>";
	RenderTextOnScreen(meshList[GEO_TEXT], ammoText.str(), Color(1, 1, 1), 2.5, 115, 7.5);
	if (IsReloading && weaponValue != 13 && weaponValue < 15)
	{
		std::stringstream RTimeText;
		RTimeText << std::fixed << std::setprecision(0) << "Currently Reloading - <" << (WepSys.ReloadTime - ReloadWaitTime) << ">";
		RenderTextOnScreen(meshList[GEO_TEXT], RTimeText.str(), Color(1, 1, 1), 2.5, 115, 4);
	}
	else if (weaponValue != 13 && weaponValue < 15)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <R> To Reload Weapon.", Color(1, 1, 1), 2.5, 115, 4);
	}

	modelStack.PopMatrix();
	//INFO UI, STATS END

	if (repairShipPhase != 4)
	{
		RenderImageOnScreen(Crosshair, 10, 10, 80, 45);
	}

	RenderWepScreen(weaponinterface, WepItf_Choices);

	RenderTeleporter(camera.teleCheck);

	RenderImageOnScreen(UI_BG, 50, 10, 2.5, 81);
	RenderTextOnScreen(meshList[GEO_TEXT], "Wave Number: " + std::to_string(wave), Color(0, 1, 1), 3, 2.5, 81);
}


/****************************************************************************/
/*!
\brief
Method that renders the rocks in the scene
*/
/****************************************************************************/

void SP2_Scene::RenderRocks()
{
	int range = 6;
	for (int detailLevel = 1; detailLevel <= range; detailLevel++)
	{
		for (int j = -6; j < 6; j += 12)
		{
			for (int i = 1; i <= 360; i += 60 - range * 5)
			{
				modelStack.PushMatrix();
				modelStack.Translate((detailLevel * 70 + 200) * cos(Math::DegreeToRadian(i + detailLevel * 20)), -30, (detailLevel * 70 + 200) * sin(Math::DegreeToRadian(i + detailLevel * 20)));
				modelStack.Rotate(5 * i, 0, 1, 0);

				modelStack.PushMatrix();
				modelStack.Rotate(20 + 5 * (i + i * detailLevel), 1, 1, 1);
				modelStack.Scale(30, 30, 20);
				//modelStack.Translate(0, moveRocks / 5, 0);
				RenderMesh(meshList[GEO_METEOR], true);
				modelStack.PopMatrix();

				modelStack.PopMatrix();
			}
		}
	}
}


/****************************************************************************/
/*!
\brief
Method that renders the main menu of the game
*/
/****************************************************************************/

void SP2_Scene::RenderMainMenu()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 5, 25);
	modelStack.Rotate(180, 0, 1, 0);

	// MAIN UI
	modelStack.PushMatrix();
	// Move UI Back

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -UIPushBackVal);
	//Logo
	modelStack.PushMatrix();
	modelStack.Translate(-6, 3, 0);
	modelStack.Rotate(10, 0, 1, 0);
	RenderImageInMap(UI_Logo, 15, 7.5);
	modelStack.PopMatrix();

	//Crystal
	if (!InSettings && !InstructionScreen)
	{
		modelStack.PushMatrix();
		modelStack.Translate(UICrystalPosition.x, UICrystalPosition.y, UICrystalPosition.z);
		modelStack.Rotate(constRotation, 0, 1, 0);
		RenderMesh(meshList[GEO_CRYSTAL], true);
		modelStack.PopMatrix();
	}

	//Choices
	modelStack.PushMatrix();
	modelStack.Rotate(-10, 0, 1, 0);
	modelStack.PushMatrix();
	modelStack.Translate(0, 4.5, 0);
	modelStack.Translate(5, 0, 0);
	if (UICrystalChoice == 1){ RenderText(meshList[GEO_TEXT], "Start Game", Color(0, 1, 1)); }
	else { RenderText(meshList[GEO_TEXT], "Start Game", Color(1, 1, 1)); }
	modelStack.Translate(2.5, 0, 0);
	RenderImageInMap(UI_LoadingBG, 8, 1.5);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	modelStack.Translate(0, 1.5, 0);
	modelStack.Translate(5, 0, 0);
	if (UICrystalChoice == 2){ RenderText(meshList[GEO_TEXT], "Instructions", Color(0, 1, 1)); }
	else { RenderText(meshList[GEO_TEXT], "Instructions", Color(1, 1, 1)); }
	modelStack.Translate(2.5, 0, 0);
	RenderImageInMap(UI_LoadingBG, 8, 1.5);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	modelStack.Translate(0, -1.5, 0);
	modelStack.Translate(5, 0, 0);
	if (UICrystalChoice == 3){ RenderText(meshList[GEO_TEXT], "Settings", Color(0, 1, 1)); }
	else { RenderText(meshList[GEO_TEXT], "Settings", Color(1, 1, 1)); }
	modelStack.Translate(2.5, 0, 0);
	RenderImageInMap(UI_LoadingBG, 8, 1.5);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	modelStack.Translate(0, -4.5, 0);
	modelStack.Translate(5, 0, 0);
	if (UICrystalChoice == 4){ RenderText(meshList[GEO_TEXT], "Quit Game", Color(0, 1, 1)); }
	else { RenderText(meshList[GEO_TEXT], "Quit Game", Color(1, 1, 1)); }
	modelStack.Translate(2.5, 0, 0);
	RenderImageInMap(UI_LoadingBG, 8, 1.5);
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	//Help
	modelStack.PushMatrix();
	modelStack.Scale(0.7, 0.7, 0.7);
	modelStack.Rotate(10, 0, 1, 0);
	modelStack.Translate(-8, -4, 0);
	RenderImageInMap(UI_LoadingBG, 16, 6);
	modelStack.Translate(-6, 2, 0);
	RenderText(meshList[GEO_TEXT], "Move Your Mouse To Choose!", Color(1, 1, 1));
	modelStack.Translate(0, -2, 0);
	RenderText(meshList[GEO_TEXT], "Left Click To Select!", Color(1, 1, 1));
	modelStack.Translate(0, -2, 0);
	RenderText(meshList[GEO_TEXT], "Right Click To Exit A Menu!", Color(1, 1, 1));
	modelStack.PopMatrix();
	modelStack.PopMatrix();
	// MAIN UI END

	if (InstructionScreen)
	{
		// INSTR UI 
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, -1);
		RenderImageInMap(UI_WepSel_BG, 30, 15);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(InstrScrnMove, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<Instructions Menu>", Color(1, 1, 1));
		modelStack.Scale(0.6, 0.6, 0.6);
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Look Right> to Scroll Right", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Look Left> to Scroll Left", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Right Click>", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "to return to Main Menu.", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.Translate(15, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<About The Game>", Color(1, 1, 1));
		modelStack.Scale(0.5, 0.5, 0.5);
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Battle the Robots that are trying", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "to get to your Base's Power Core!", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "The Robots will hurt you if you touch them.", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "You will lose if the Robots get to the core", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "or if you run out of health points.", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.Translate(15, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<Player Bindings>", Color(1, 1, 1));
		modelStack.Scale(0.6, 0.6, 0.6);
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<WASD Keys> to Walk", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Hold <Shift> to Run", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Space Bar> to Jump", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Left Click> to Shoot", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Right Click> to Zoom [Sniper Only]", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<R> to Reload Weapon", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.Translate(15, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<Weapons & Their Types>", Color(1, 1, 1));
		modelStack.Scale(0.6, 0.6, 0.6);
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Rifle> Medium Fire Rate and Power", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Sniper> Low Fire Rate and High Power", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<SMG> High Fire Rate and Low Power", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Type: Damage> Increased Power", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Type: Capacity> Increased Ammo Amount", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "<Type: Speed> Increased Fire Rate", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.Translate(15, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<About The Base>", Color(1, 1, 1));
		modelStack.Scale(0.5, 0.5, 0.5);
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "A 2 Storey Building with openings on each side", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "A Power Core exists in the center of floor 1", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "There are teleporters in the corners of floor 1", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "The teleporters let you get to floor 2", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Power-Ups spawn on floor 2 every round", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.Translate(15, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<About Power-Ups>", Color(1, 1, 1));
		modelStack.Scale(0.5, 0.5, 0.5);
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Power-Ups change the type of gun you have.", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Depending on the Power-Up's color,", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "your gun will change to the color", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "of that gun type.", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Red - Damage, Blue - Capacity, Green - Speed", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.Translate(15, 0, 0);
		modelStack.PushMatrix();
		RenderImageInMap(UI_BG, 14, 14);
		modelStack.Translate(-5, 5, 0.3);
		RenderText(meshList[GEO_TEXT], "<About Shooting>", Color(1, 1, 1));
		modelStack.Scale(0.5, 0.5, 0.5);
		modelStack.Translate(0, -2.5, 0);
		RenderText(meshList[GEO_TEXT], "You will fight the Robots with Laser Weapons.", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "These Lasers are able to phase through Robots.", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "Line the Robots up and shoot them to maximize", Color(1, 1, 1));
		modelStack.Translate(0, -3, 0);
		RenderText(meshList[GEO_TEXT], "the damage you deal to the Robots!", Color(1, 1, 1));
		modelStack.PopMatrix();
		modelStack.PopMatrix();
		// INSTR UI END
	}

	else if (InSettings)
	{
		//Crystal
		modelStack.PushMatrix();
		modelStack.Translate(UICrystalPosition.x - 12, UICrystalPosition.y, UICrystalPosition.z);
		modelStack.Rotate(constRotation, 0, 1, 0);
		RenderMesh(meshList[GEO_CRYSTAL], true);
		modelStack.PopMatrix();

		// SETTINGS UI 
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, -2);
		RenderImageInMap(UI_BG, 30, 15);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(InstrScrnMove, 0, 0);
		modelStack.PushMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-4, 5, 0);
		RenderText(meshList[GEO_TEXT], "<Settings Menu>", Color(0, 1, 1));
		modelStack.Translate(3.4, 0, 0);
		RenderImageInMap(UI_LoadingBG, 10, 1.5);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-6, 2, 0);
		if (UICrystalChoice2 == 1){ RenderText(meshList[GEO_TEXT], "Mouse Sensitivity", Color(0, 1, 1)); }
		else{ RenderText(meshList[GEO_TEXT], "Mouse Sensitivity", Color(1, 1, 1)); }
		modelStack.Translate(4, 0, 0);
		RenderImageInMap(UI_LoadingBG, 11, 1.5);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(8, 2, 0);
		if (camera.MouseSensitivity == 1){ RenderText(meshList[GEO_TEXT], "Normal", Color(1, 1, 0)); }
		else if (camera.MouseSensitivity == 2){ RenderText(meshList[GEO_TEXT], "High", Color(1, 0, 0)); }
		else if (camera.MouseSensitivity == 0.5){ RenderText(meshList[GEO_TEXT], "Low", Color(0, 1, 0)); }
		modelStack.Translate(1, 0, 0);
		RenderImageInMap(UI_LoadingBG, 5, 1.5);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-6, -1, 0);
		if (UICrystalChoice2 == 2){ RenderText(meshList[GEO_TEXT], "Testing Mode", Color(0, 1, 1)); }
		else{ RenderText(meshList[GEO_TEXT], "Testing Mode", Color(1, 1, 1)); }
		modelStack.Translate(3, 0, 0);
		RenderImageInMap(UI_LoadingBG, 9, 1.5);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, -1, 0);
		if (TestingModeON){ RenderText(meshList[GEO_TEXT], "Yes", Color(0, 1, 0)); }
		else if (!TestingModeON){ RenderText(meshList[GEO_TEXT], "No", Color(1, 0, 0)); }
		modelStack.Translate(1, 0, 0);
		RenderImageInMap(UI_LoadingBG, 5, 1.5);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-6, -4, 0);
		if (UICrystalChoice2 == 3){ RenderText(meshList[GEO_TEXT], "Game Volume Level", Color(0, 1, 1)); }
		else{ RenderText(meshList[GEO_TEXT], "Game Volume Level", Color(1, 1, 1)); }
		modelStack.Translate(4.5, 0, 0);
		RenderImageInMap(UI_LoadingBG, 12, 1.5);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, -4, 0);
		if (VolumeLevel == 1){ RenderText(meshList[GEO_TEXT], "Normal", Color(1, 1, 0)); }
		else if (VolumeLevel == 2){ RenderText(meshList[GEO_TEXT], "High", Color(1, 0, 0)); }
		else if (VolumeLevel == 0){ RenderText(meshList[GEO_TEXT], "Low", Color(0, 1, 0)); }
		modelStack.Translate(1, 0, 0);
		RenderImageInMap(UI_LoadingBG, 5, 1.5);
		modelStack.PopMatrix();

		modelStack.PopMatrix();
		// SETTINGS UI END
	}


	modelStack.PopMatrix();
}

/****************************************************************************/
/*!
\brief
Method that renders the loading screen of the game
*/
/****************************************************************************/

void SP2_Scene::RenderLoadScreen()
{
	RenderImageOnScreen(UI_LoadingBG, 160, 90, 80, 45);
	RenderTextOnScreen(meshList[GEO_TEXT], "Game Currently Loading", Color(0, 0.5, 1), 8, 15, 80);
	RenderTextOnScreen(meshList[GEO_TEXT], "Please Wait", Color(0, 0.5, 1), 8, 45, 70);
	if (!loadRobots || !loadMap || !loadWep)
	{
		RenderImageOnScreen(UI_HP_Red, 130, 8, 80, 40);
		RenderImageOnScreen(UI_HP_Green, 130 * (LoadTimer / MaxLoadTime), 8, 80, 40);
		RenderImageOnScreen(UI_LoadingBarOverlay, 130, 8, 80, 40);
	}
	else{
		RenderTextOnScreen(meshList[GEO_TEXT], "<Loading Completed>", Color(0, 0.5, 1), 6, 50, 45);
	}
}

/****************************************************************************/
/*!
\brief
Method that renders the game over screen of the game
*/
/****************************************************************************/

void SP2_Scene::RenderGameOver()
{
	RenderImageOnScreen(UI_LoadingBG, 160, 90, 80, 45);
	if (basehp > 0 && playerhp > 0)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<Game Complete>", Color(0, 0.5, 1), 10, 13, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], "You have successfully completed the waves!", Color(0, 0.5, 1), 6, 13, 55);
	}
	else{
		RenderTextOnScreen(meshList[GEO_TEXT], "<Game Over>", Color(1, 0, 0), 10, 13, 75);
		RenderTextOnScreen(meshList[GEO_TEXT], "You have failed to complete the waves...", Color(1, 0, 0), 6, 13, 55);
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "Click the <Right Mouse Button>", Color(0, 1, 1), 5, 15, 30);
	RenderTextOnScreen(meshList[GEO_TEXT], "to return to the Main Menu", Color(0, 1, 1), 5, 15, 25);
}

/****************************************************************************/
/*!
\brief
Method that renders everything in the game
*/
/****************************************************************************/

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
	RenderSkybox(camera.getCameraPosition());
	if (GameLoading)
	{
		RenderLoadScreen();
	}
	else if (GameOver)
	{
		RenderGameOver();
	}
	else if (InMainMenu)
	{
		RenderMainMenu();
	}
	else{
		readtextfile();

		//Render In-Hand Weapon
		modelStack.PushMatrix();
		RenderWeaponInHand(weaponValue, 5, 1, 1);
		modelStack.PopMatrix();

		for (auto i : WepSys.BulletList)
		{
			i.Move();
			modelStack.PushMatrix();
			modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_BULLET], false);
			modelStack.PopMatrix();
		}

		GameState();

		//DO NOT RENDER ANYTHING UNDER THIS//
		RenderGameUI();
	}
}

/****************************************************************************/
/*!
\brief
Method that closes the program
*/
/****************************************************************************/

void SP2_Scene::Exit()
{
	engine->drop();
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}