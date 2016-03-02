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
	"Sounds//PhaserShoot.ogg",
	"Sounds//Gnosemite.ogg",
	"Sounds//Cobra2.ogg",
	"Sounds//Viper_Orchestral.ogg"
};

ISound* ShootSound;
ISound* WaveBGM;
ISound* WaveBGM2;
ISound* MenuBGM;

int pause = 1;

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

void SP2_Scene::Init()
{
	engine = createIrrKlangDevice();
	engine->addSoundSourceFromFile(SoundName[0].c_str());
	engine->addSoundSourceFromFile(SoundName[1].c_str());

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
	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", white, 1, 0.5, 20, 20);
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
	Crosshair2 = LoadTGA("Image//CockpitCHair.tga");
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

}

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

	meshList[GEO_SHOTGUN] = MeshBuilder::GenerateOBJ("test", "OBJ//Shotgun.obj");
	meshList[GEO_SHOTGUN]->textureID = LoadTGA("Image//Tex_Shotgun.tga");

	
}

void SP2_Scene::InitMapModels()
{
	meshList[GEO_CRYSTALBASE] = MeshBuilder::GenerateOBJ("Core Platform", "OBJ//Core_Platform.obj");
	meshList[GEO_CRYSTALBASE]->textureID = LoadTGA("Image//Tex_Core_Platform.tga");

	meshList[GEO_CRYSTAL] = MeshBuilder::GenerateOBJ("Core Crystal", "OBJ//Core_Crystal.obj");
	meshList[GEO_CRYSTAL]->textureID = LoadTGA("Image//Tex_LightOrb.tga");

	meshList[GEO_MOONFLOOR] = MeshBuilder::GenerateOBJ("Baseplate", "OBJ//Moon_Floor.obj");
	meshList[GEO_MOONFLOOR]->textureID = LoadTGA("Image//Tex_Moon.tga");

	meshList[GEO_TELEPORTER] = MeshBuilder::GenerateOBJ("Teleporter", "OBJ//Teleporter.obj");
	meshList[GEO_TELEPORTER]->textureID = LoadTGA("Image//Tex_Lightorb.tga");

	meshList[GEO_GATE] = MeshBuilder::GenerateOBJ("test", "OBJ//Gate_Door.obj");
	meshList[GEO_GATE]->textureID = LoadTGA("Image//Tex_Gate.tga");

	meshList[GEO_BASE] = MeshBuilder::GenerateOBJ("base", "OBJ//base.obj");
	meshList[GEO_BASE]->textureID = LoadTGA("Image//Tex_Gate2.tga");

	meshList[GEO_METEOR] = MeshBuilder::GenerateOBJ("test", "OBJ//meteor.obj");
	meshList[GEO_METEOR]->textureID = LoadTGA("Image//Tex_Meteor.tga");

	//meshList[GEO_PLAYERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//PlayerShip.obj");
	//meshList[GEO_PLAYERSHIP]->textureID = LoadTGA("Image//Tex_PlayerShip.tga");

	meshList[GEO_MOTHERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership.obj");
	meshList[GEO_MOTHERSHIP]->textureID = LoadTGA("Image//Tex_Mothership.tga");

	/*meshList[GEO_COMPUTER] = MeshBuilder::GenerateOBJ("test", "OBJ//computer.obj");
	meshList[GEO_COMPUTER]->textureID = LoadTGA("Image//computer.tga");*/
}

void SP2_Scene::InitRobots()
{
	meshList[GEO_ROBOTHEALTH] = MeshBuilder::GenerateCube("cube", Color(0, 1, 0));
	meshList[GEO_ROBOTHEALTH2] = MeshBuilder::GenerateCube("cube", Color(1, 0, 0));
	
	//drone
	meshList[GEO_DRONEBODY] = MeshBuilder::GenerateOBJ("test", "OBJ//Drone_body.obj");
	GLuint texGD = LoadTGA("Image//Tex_Drone.tga");
	meshList[GEO_DRONEBODY]->textureID = texGD;
	meshList[GEO_DRONELEFTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Drone_leftupperarm.obj");
	meshList[GEO_DRONELEFTUPPERARM]->textureID = texGD;
	meshList[GEO_DRONELEFTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Drone_leftlowerarm.obj");
	meshList[GEO_DRONELEFTLOWERARM]->textureID = texGD;
	meshList[GEO_DRONERIGHTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Drone_rightupperarm.obj");
	meshList[GEO_DRONERIGHTUPPERARM]->textureID = texGD;
	meshList[GEO_DRONERIGHTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//Drone_rightlowerarm.obj");
	meshList[GEO_DRONERIGHTLOWERARM]->textureID = texGD;

	//melee robot
	meshList[GEO_MELEEROBOTBODY] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_body.obj");
	GLuint texGD1 = LoadTGA("Image//Tex_Robot1.tga");
	meshList[GEO_MELEEROBOTBODY]->textureID = texGD1;
	meshList[GEO_MELEEROBOTLEFTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_leftupperarm.obj");
	meshList[GEO_MELEEROBOTLEFTUPPERARM]->textureID = texGD1;
	meshList[GEO_MELEEROBOTLEFTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_leftlowerarm.obj");
	meshList[GEO_MELEEROBOTLEFTLOWERARM]->textureID = texGD1;
	meshList[GEO_MELEEROBOTLEFTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_leftleg.obj");
	meshList[GEO_MELEEROBOTLEFTLEG]->textureID = texGD1;
	meshList[GEO_MELEEROBOTRIGHTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_rightupperarm.obj");
	meshList[GEO_MELEEROBOTRIGHTUPPERARM]->textureID = texGD1;
	meshList[GEO_MELEEROBOTRIGHTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_rightlowerarm.obj");
	meshList[GEO_MELEEROBOTRIGHTLOWERARM]->textureID = texGD1;
	meshList[GEO_MELEEROBOTRIGHTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//MeleeRobot_rightleg.obj");
	meshList[GEO_MELEEROBOTRIGHTLEG]->textureID = texGD1;

	//range robot
	meshList[GEO_RANGEROBOTBODY] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_body.obj");
	GLuint texGD2 = LoadTGA("Image//Tex_Robot2.tga");
	meshList[GEO_RANGEROBOTBODY]->textureID = texGD2;
	meshList[GEO_RANGEROBOTLEFTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_leftupperarm.obj");
	meshList[GEO_RANGEROBOTLEFTUPPERARM]->textureID = texGD2;
	meshList[GEO_RANGEROBOTLEFTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_leftlowerarm.obj");
	meshList[GEO_RANGEROBOTLEFTLOWERARM]->textureID = texGD2;
	meshList[GEO_RANGEROBOTLEFTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_leftleg.obj");
	meshList[GEO_RANGEROBOTLEFTLEG]->textureID = texGD2;
	meshList[GEO_RANGEROBOTRIGHTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_rightupperarm.obj");
	meshList[GEO_RANGEROBOTRIGHTUPPERARM]->textureID = texGD2;
	meshList[GEO_RANGEROBOTRIGHTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_rightlowerarm.obj");
	meshList[GEO_RANGEROBOTRIGHTLOWERARM]->textureID = texGD2;
	meshList[GEO_RANGEROBOTRIGHTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//RangeRobot_rightleg.obj");
	meshList[GEO_RANGEROBOTRIGHTLEG]->textureID = texGD2;

	//mixed robot (melee and range)
	meshList[GEO_MIXEDROBOTBODY] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_body.obj");
	GLuint texGD3 = LoadTGA("Image//Tex_Robot3.tga");
	meshList[GEO_MIXEDROBOTBODY]->textureID = texGD3;
	meshList[GEO_MIXEDROBOTLEFTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_leftupperarm.obj");
	meshList[GEO_MIXEDROBOTLEFTUPPERARM]->textureID = texGD3;
	meshList[GEO_MIXEDROBOTLEFTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_leftlowerarm.obj");
	meshList[GEO_MIXEDROBOTLEFTLOWERARM]->textureID = texGD3;
	meshList[GEO_MIXEDROBOTLEFTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_leftleg.obj");
	meshList[GEO_MIXEDROBOTLEFTLEG]->textureID = texGD3;
	meshList[GEO_MIXEDROBOTRIGHTUPPERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_rightupperarm.obj");
	meshList[GEO_MIXEDROBOTRIGHTUPPERARM]->textureID = texGD3;
	meshList[GEO_MIXEDROBOTRIGHTLOWERARM] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_rightlowerarm.obj");
	meshList[GEO_MIXEDROBOTRIGHTLOWERARM]->textureID = texGD3;
	meshList[GEO_MIXEDROBOTRIGHTLEG] = MeshBuilder::GenerateOBJ("test", "OBJ//MixedRobot_rightleg.obj");
	meshList[GEO_MIXEDROBOTRIGHTLEG]->textureID = texGD3;
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

void SP2_Scene::RenderRobots()
{
	//for (auto i : RobotManager.RobotList)
	//{
	//	modelStack.PushMatrix();
	//	//i.BoundsCheck(WepSys.BulletList);
	//	modelStack.Translate(i.Position().x, i.Position().y + 0.5, i.Position().z);
	//	modelStack.Rotate(i.rotateToTarget, 0, 1, 0);
	//	modelStack.Rotate(-45, 0, 1, 0);
	//	RenderMesh(meshList[GEO_MELEEROBOTBODY], true);
	//	modelStack.PushMatrix();
	//	modelStack.Rotate(rotatelefthand, 1, 0, 0);
	//	modelStack.Translate(0, 0, -10);
	//	modelStack.Translate(0, 0, 10);
	//	modelStack.Translate(0.3, 0, 0);
	//	RenderMesh(meshList[GEO_MELEEROBOTLEFTUPPERARM], true);
	//	modelStack.PushMatrix();
	//	modelStack.Translate(0, 0, leftarmattack);
	//	modelStack.Rotate(rotatelefthand, 1, 0, 0);
	//	RenderMesh(meshList[GEO_MELEEROBOTLEFTLOWERARM], true);
	//	modelStack.PopMatrix();
	//	modelStack.PopMatrix();
	//	modelStack.PushMatrix();
	//	modelStack.Rotate(rotaterighthand, 1, 0, 0);
	//	modelStack.Translate(0, 0, -10);
	//	modelStack.Translate(0, 0, 10);
	//	modelStack.Translate(-0.3, 0, 0);
	//	RenderMesh(meshList[GEO_MELEEROBOTRIGHTUPPERARM], true);
	//	modelStack.PushMatrix();
	//	modelStack.Translate(0, 0, rightarmattack);
	//	modelStack.Rotate(rotaterighthand, 1, 0, 0);
	//	RenderMesh(meshList[GEO_MELEEROBOTRIGHTLOWERARM], true);
	//	modelStack.PopMatrix();
	//	modelStack.PopMatrix();
	//	modelStack.PushMatrix();
	//	modelStack.Rotate(moveleftleg, 1, 0, 0);
	//	modelStack.Translate(0, 0, -10);
	//	modelStack.Translate(0, 0, 10);
	//	RenderMesh(meshList[GEO_MELEEROBOTLEFTLEG], true);
	//	modelStack.PopMatrix();
	//	modelStack.PushMatrix();
	//	modelStack.Rotate(moverightleg, 1, 0, 0);
	//	RenderMesh(meshList[GEO_MELEEROBOTRIGHTLEG], true);
	//	modelStack.PopMatrix();
	//	modelStack.PopMatrix();
	//}
}

void SP2_Scene::GameState()
{
	if (basehp == 0 || playerhp == 0)
	{
		if (wave != 7)
		{
			WaveBGM->stop();
		}
		else WaveBGM2->stop();
		WepSys.ClearList();
		EnemyWepSys.ClearList();
		RobotManager.ClearList();
		MothershipHandler.ClearList();
		GameOver = true;
		wave = 0;
	}
	else if (wave == 1 && !weaponinterface && !SpawnedRobots)
	{
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(0, 0, Vector3(spawnPointN.x + (rand() % 80 - 39), spawnPointN.y, spawnPointN.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 2 && !weaponinterface && !SpawnedRobots)
	{
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(1, 180, Vector3(spawnPointS.x + (rand() % 80 - 39), spawnPointS.y, spawnPointS.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 3 && !weaponinterface && !SpawnedRobots)
	{
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, -90, Vector3(spawnPointE.x + (rand() % 80 - 39), spawnPointE.y, spawnPointE.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 4 && !weaponinterface && !SpawnedRobots)
	{
		for (int i = 0; i < 10; i++)
		{
			RobotManager.RobotList.push_back(Robot(2, 90, Vector3(spawnPointW.x + (rand() % 80 - 39), spawnPointW.y, spawnPointW.z + (rand() % 80 - 39))));
		}
		SpawnedRobots = true;
	}
	else if (wave == 5 && !weaponinterface && !SpawnedRobots)
	{
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
		RenderBase();
		RenderShip();
		//RenderGate();
		//RenderLevel(); //May not need this
		camera.spaceModeOn = false;
		if (!PlayBGM)
		{
			WaveBGM = engine->play2D(SoundName[1].c_str(), true, false, true);
			WaveBGM->setVolume(0.1);
			PlayBGM = true;
			beforeWave7 = true;
		}
		
	}
	else if (wave == 7)
	{
		if (ShipSpawned == false)
		{
			if (beforeWave7){ WaveBGM->stop(); PlayBGM = false; beforeWave7 = false; }
			if (!PlayBGM)
			{
				WaveBGM2 = engine->play2D(SoundName[2].c_str(), true, false, true);
				WaveBGM2->setVolume(0.1);
				PlayBGM = true;
			}
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

void SP2_Scene::RenderLevel()
{
	if (wave == 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 1", Color(1, 0, 0), 3, 72.5, 87);

		if (RobotManager.CurrentRobotCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 1 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}

	if (wave == 2)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 2", Color(1, 0, 0), 3, 72.5, 87);

		if (RobotManager.CurrentRobotCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 2 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}
	if (wave == 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 3", Color(1, 0, 0), 3, 72.5, 87);

		if (RobotManager.CurrentRobotCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 3 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}
	if (wave == 4)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 4", Color(1, 0, 0), 3, 72.5, 87);

		if (RobotManager.CurrentRobotCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 4 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}
	if (wave == 5)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 5", Color(1, 0, 0), 3, 72.5, 87);

		if (RobotManager.CurrentRobotCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 5 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}
}
void SP2_Scene::RenderSpaceMap()
{
	for (auto i : MothershipHandler.RobotList)
	{
		modelStack.PushMatrix();
		modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
		modelStack.Rotate(ShipRotation, 0, 1, 0);
			modelStack.PushMatrix();
			//modelStack.Translate(0, 10, 100);
			//modelStack.Translate(0, moveMshipUp, 0);
			modelStack.Scale(20, 20, 20);
			RenderMesh(meshList[GEO_MOTHERSHIP], true);
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
	int range = 5;
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
			MenuBGM->setVolume(0.1);
			PlayBGM = true;
		}
		light[1].color.Set(0.3, 0.95, 1);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		if (!buttonPress)
		{
			buttonValue += dt;
			if (buttonValue >= 1)
			{
				buttonPress = true;
			}
		}
		camera.CanMoveCamera = false;
		camera.maxCameraXrotation = 5;
		camera.minCameraXrotation = -5;
		camera.maxCameraYrotation = 5;
		camera.minCameraYrotation = -5;
		UICrystalPosition.y = -5 - camera.CameraXrotation;
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
		if (buttonPress && UICrystalChoice == 1 && InMainMenu && Application::IsKeyPressed(VK_LBUTTON))
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
		}
		else if (buttonPress && UICrystalChoice == 2 && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			buttonPress = false;
			buttonValue = 0;
		}
		else if (buttonPress && UICrystalChoice == 3 && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			buttonPress = false;
			buttonValue = 0;
			InSettings = true;
		}
		else if (buttonPress && UICrystalChoice == 4 && Application::IsKeyPressed(VK_LBUTTON))
		{
			light[1].color.Set(0, 0, 0);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			buttonPress = false;
			buttonValue = 0;
			MenuBGM->stop();
			PlayBGM = false;
			Application::CloseGame = true;
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

		if (Application::IsKeyPressed('I'))
		{
			weaponinterface = true;
		}
		if (Application::IsKeyPressed('U'))
		{
			weaponinterface = false;
		}
		//timer += (float)(1 * dt);

		if (Application::IsKeyPressed('P') && pause == 1)
		{
			pause = 0;
			buttonPress = false;
			buttonValue = 0;
		}
		else if (Application::IsKeyPressed('O') && pause == 0)
		{
			pause = 1;
			buttonPress = false;
			buttonValue = 0;
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
		}
		if (camera.shipCheck == true && Application::IsKeyPressed('E') && repairShipPhase == 2)
		{
			repairShipPhase = 3;
			weaponValue = 15;
		}
		if (camera.shipCheck == true && Application::IsKeyPressed('F') && repairShipPhase == 3)
		{
			repairShipPhase = 4;
			weaponValue = 0;
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
		if (!IsReloading && CanFire && weaponValue != 13 && weaponValue <= 14 && Application::IsKeyPressed(VK_LBUTTON))
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
				ShootSound->setVolume(0.1);
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

		/*if (CanFire && Application::IsKeyPressed(VK_RBUTTON))
		{
			RobotManager.RobotList.push_back(Robot(0, spawnPointN));
			RobotManager.RobotList.push_back(Robot(0, spawnPointE));
			RobotManager.RobotList.push_back(Robot(0, spawnPointS));
			RobotManager.RobotList.push_back(Robot(0, spawnPointW));
			CanFire = false;
			GunWaitTime = 0;
		}*/

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
		if (ShipSpawned && weaponsOn && MothershipHandler.RobotList.front().GetHealth() >= 0)
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
		else if (ShipSpawned && MothershipHandler.RobotList.front().GetHealth() <= 0)
		{
			WaveBGM2->stop();
			GameOver = true;
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

void SP2_Scene::RenderGateText(bool render)
{
	if (render)
	{
		modelStack.PushMatrix();
		RenderImageOnScreen(UI_BG, 30, 2.75, 80, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <E> to open", Color(0.000f, 0.808f, 0.820f), 2.5, 68.5, 25);
		modelStack.PopMatrix();
	}
}

void SP2_Scene::RenderUI()
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
	ammoText << std::fixed << std::setprecision(0) << "Ammo - < " << CurrentAmmo << " / " << WepSys.MaxAmmo << " >";
	RenderTextOnScreen(meshList[GEO_TEXT], ammoText.str(), Color(1, 1, 1), 2.5, 115, 7.5);
	if (IsReloading)
	{
		std::stringstream RTimeText;
		RTimeText << std::fixed << std::setprecision(0) << "Currently Reloading - <" << (WepSys.ReloadTime - ReloadWaitTime) << ">";
		RenderTextOnScreen(meshList[GEO_TEXT], RTimeText.str(), Color(1, 1, 1), 2.5, 115, 4);
	}
	else 
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press <R> To Reload Weapon.", Color(1, 1, 1), 2.5, 115, 4);
	}
	/*std::stringstream coordText;
	coordText << std::fixed << std::setprecision(1) << "Player Location = (" << camera.getCameraPosition().x << ", " << camera.getCameraPosition().y << ", " << camera.getCameraPosition().z << ")";
	RenderTextOnScreen(meshList[GEO_TEXT], coordText.str(), Color(1, 1, 1), 2.5, 3, 4);*/
	modelStack.PopMatrix();
	//INFO UI, STATS END

	if (repairShipPhase != 4)
	{
		RenderImageOnScreen(Crosshair, 10, 10, 80, 45);
	}

	RenderWepScreen(weaponinterface, WepItf_Choices);

	RenderTeleporter(camera.teleCheck);

	//RenderGateText(camera.checkLeftGate || camera.checkRightGate || camera.checkFrontGate || camera.checkBackGate);

	//RenderTextOnScreen(meshList[GEO_TEXT], "Base HP: " + std::to_string(basehp), Color(0, 0.5, 0), 3, 2.5, 87);

	//RenderTextOnScreen(meshList[GEO_TEXT], "Ammo: " + std::to_string(ammo), Color(0, 0.5, 0), 3, 2.5, 84);

	RenderImageOnScreen(UI_BG, 50, 10, 2.5, 81);
	RenderTextOnScreen(meshList[GEO_TEXT], "Wave Number: " + std::to_string(wave), Color(0, 1, 1), 3, 2.5, 81);

	if (pause == 0)
	{
		modelStack.PushMatrix();
		RenderImageOnScreen(UI_BG, 50, 10, 80, 45);
		RenderTextOnScreen(meshList[GEO_TEXT], "Paused", Color(0, 0.5, 0), 10, 68, 45);
		modelStack.PopMatrix();
	}
}

void SP2_Scene::RenderGate(bool render)
{
	modelStack.PushMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(17.2, 0.5, camera.leftGateA);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(17.2, 0.5, camera.leftGateB);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-17.2, 0.5, camera.rightGateA);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-17.2, 0.5, camera.rightGateB);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.backGateA, 0.5, -17.2);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.backGateB, 0.5, -17.2);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	modelStack.PushMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.frontGateA, 0.5, 17.2);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.frontGateB, 0.5, 17.2);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SP2_Scene::RenderRocks()
{
	//// Right
	//modelStack.PushMatrix();
	//modelStack.Translate(45, -2, -30);
	//modelStack.Rotate(0, 0, 0, 1);
	//modelStack.Scale(4, 4, 4);
	//RenderMesh(meshList[GEO_METEOR], true);

	//	modelStack.PushMatrix();
	//	modelStack.Translate(4, -2, -8);
	//	modelStack.Rotate(50, 1, 0, 0);
	//	modelStack.Scale(3, 3, 3);
	//	RenderMesh(meshList[GEO_METEOR], true);

	//		modelStack.PushMatrix();
	//		modelStack.Translate(3, -2, 20);
	//		modelStack.Rotate(30, 1, 0, 0);
	//		modelStack.Scale(-1, -1, -1);
	//		RenderMesh(meshList[GEO_METEOR], true);

	//modelStack.PopMatrix();
	//modelStack.PopMatrix();
	//modelStack.PopMatrix();

	//// left
	//modelStack.PushMatrix();
	//modelStack.Translate(-50, -2, -20);
	//modelStack.Rotate(-40, 0, 0, 1);
	//modelStack.Scale(4, 4, 4);
	//RenderMesh(meshList[GEO_METEOR], true);

	//	modelStack.PushMatrix();
	//	modelStack.Translate(-10, -10, -8);
	//	modelStack.Rotate(50, 0, 0, 1);
	//	modelStack.Scale(3, 3, 3);
	//	RenderMesh(meshList[GEO_METEOR], true);

	//		modelStack.PushMatrix();
	//		modelStack.Translate(-1, -2, -5);
	//		modelStack.Rotate(-20, 0, 1, 0);
	//		modelStack.Scale(1, 1, 1);
	//		RenderMesh(meshList[GEO_METEOR], true);

	//modelStack.PopMatrix();
	//modelStack.PopMatrix();
	//modelStack.PopMatrix();

	//// front
	//modelStack.PushMatrix();
	//modelStack.Translate(25, -2, -60);
	//modelStack.Rotate(50, 0, 0, 1);
	//modelStack.Scale(4, 4, 4);
	//RenderMesh(meshList[GEO_METEOR], true);

	//modelStack.PushMatrix();
	//modelStack.Translate(4, -4, -4);
	//modelStack.Rotate(20, 0, 1, 0);
	//modelStack.Scale(2, 2, 2);
	//RenderMesh(meshList[GEO_METEOR], true);

	//modelStack.PopMatrix();
	//modelStack.PopMatrix();

	//// back
	//modelStack.PushMatrix();
	//modelStack.Translate(30, -2, 60);
	//modelStack.Rotate(-30, 0, 0, 1);
	//modelStack.Scale(4, 4, 4);
	//RenderMesh(meshList[GEO_METEOR], true);

	//	modelStack.PushMatrix();
	//	modelStack.Translate(5, -2, 9);
	//	modelStack.Rotate(80, 1, 0, 0);
	//	modelStack.Scale(2, 2, 2);
	//	RenderMesh(meshList[GEO_METEOR], true);

	//		modelStack.PushMatrix();
	//		modelStack.Translate(-8, -2, 20);
	//		RenderMesh(meshList[GEO_METEOR], true);

	//modelStack.PopMatrix();
	//modelStack.PopMatrix();
	//modelStack.PopMatrix();
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
	if (skyboxID == 0)
	{
		RenderSkybox(camera.getCameraPosition());
	}

	if (GameLoading)
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
			//RenderTextOnScreen(meshList[GEO_TEXT], "<Click Left Mouse Button To Start Game>", Color(0, 0.5, 1), 5, 33, 35);
		}
	}
	else if (GameOver)
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
	else if (InMainMenu)
	{
		modelStack.PushMatrix();
		//RenderMesh(meshList[GEO_AXES], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		// Move UI Back
		modelStack.Translate(0, 0, 25);
		modelStack.Rotate(180, 0, 1, 0);

		modelStack.PushMatrix();
		modelStack.Translate(0, 5, 0);
		//Logo
			modelStack.PushMatrix();
				modelStack.Translate(-6, 4, 0);
				modelStack.Rotate(10, 0, 1, 0);
				RenderImageInMap(UI_Logo, 15, 7.5);
			modelStack.PopMatrix();

		//Crystal
		modelStack.PushMatrix();
			modelStack.Translate(UICrystalPosition.x, UICrystalPosition.y, UICrystalPosition.z);
			modelStack.Rotate(constRotation, 0, 1, 0);
			RenderMesh(meshList[GEO_CRYSTAL], true);
		modelStack.PopMatrix();

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
		modelStack.PopMatrix();

		//Help
		modelStack.PushMatrix();
			modelStack.Rotate(10, 0, 1, 0);
			modelStack.Translate(-6, 2, 0);
			RenderImageInMap(UI_LoadingBG, 15, 5);
			modelStack.Translate(-6, 1, 0);
			RenderText(meshList[GEO_TEXT], "Move Your Mouse To Choose!", Color(1, 1, 1));
			modelStack.Translate( 0, -2, 0);
			RenderText(meshList[GEO_TEXT], "Left Click To Select!", Color(1, 1, 1));
		modelStack.PopMatrix();
		
		modelStack.PopMatrix();
	}
	else{
		readtextfile();

		//Render In-Hand Weapon
		modelStack.PushMatrix();
		RenderWeaponInHand(weaponValue, 5, 1, 1);
		modelStack.PopMatrix();

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
			RenderMesh(meshList[GEO_RANGEROBOTBODY], true);
				modelStack.PushMatrix();
				modelStack.Rotate(i.rotatelefthand, 1, 0, 0);
				modelStack.Translate(0, 0, -10);
				modelStack.Translate(0, 0, 10);
				modelStack.Translate(0.3, 0, 0);
				RenderMesh(meshList[GEO_RANGEROBOTLEFTUPPERARM], true);
					modelStack.PushMatrix();
					modelStack.Translate(0, 0, i.leftarmattack);
					modelStack.Rotate(i.rotatelefthand, 1, 0, 0);
					RenderMesh(meshList[GEO_RANGEROBOTLEFTLOWERARM], true);
					modelStack.PopMatrix();
				modelStack.PopMatrix();
						modelStack.PushMatrix();
						modelStack.Rotate(i.rotaterighthand, 1, 0, 0);
						modelStack.Translate(0, 0, -10);
						modelStack.Translate(0, 0, 10);
						modelStack.Translate(-0.3, 0, 0);
						RenderMesh(meshList[GEO_RANGEROBOTRIGHTUPPERARM], true);
							modelStack.PushMatrix();
							modelStack.Translate(0, 0, i.rightarmattack);
							modelStack.Rotate(i.rotaterighthand, 1, 0, 0);
							RenderMesh(meshList[GEO_RANGEROBOTRIGHTLOWERARM], true);
							modelStack.PopMatrix();
						modelStack.PopMatrix();
								modelStack.PushMatrix();
								modelStack.Rotate(i.moveleftleg, 1, 0, 0);
								modelStack.Translate(0, 0, -10);
								modelStack.Translate(0, 0, 10);
								RenderMesh(meshList[GEO_RANGEROBOTLEFTLEG], true);
								modelStack.PopMatrix();
									modelStack.PushMatrix();
									modelStack.Rotate(i.moverightleg, 1, 0, 0);
									RenderMesh(meshList[GEO_RANGEROBOTRIGHTLEG], true);
									modelStack.PopMatrix();
			modelStack.PopMatrix();
			modelStack.PopMatrix();
		}

		for (auto i : WepSys.BulletList)
		{
			modelStack.PushMatrix();
			modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_BULLET], false);
			modelStack.PopMatrix();
		}
		GameState();
		//DO NOT RENDER ANYTHING UNDER THIS//
		RenderUI();
	}
}

void SP2_Scene::Exit()
{
	engine->drop();
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

