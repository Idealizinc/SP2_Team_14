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
	"Sounds//Spacebattle.mp3"
};

ISound* Song;

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
	/*engine = createIrrKlangDevice();
	engine->addSoundSourceFromFile(SoundName[0].c_str());
	Song = engine->play2D(SoundName[0].c_str(), true, false, true);
	Song->setVolume(0.25);*/
	// Init VBO here
	glClearColor(0.15f, 0.2f, 0.35f, 0.0f);

	Mtx44 projection;
	projection.SetToPerspective(40.0f, static_cast < float >(S_Width) / static_cast < float >(S_Height), 0.1f, 3000.0f);
	projectionStack.LoadMatrix(projection);

	camera.Init(Vector3(0, 5, 3), Vector3(0, 5, 0), Vector3(0, 1, 0), 20);
	// 0, 4, 8.5
	//Initiallising Variables For Translate, Rotate, Scale
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
	gatehp = 10;
	bosshp = 100;
	playerhp = 100;
	ammo = 100;
	wave = 1;
	state = 0;
	timer = 0;
	weaponValue = 0;
	weaponinterface = false;
	//repairgate = false;
	buttonPress = true;
	buttonValue = 0;
	//droidrepair = false;
	droidrepairgate = 0;
	shipFallingX = -20;
	shipFallingY = 150;
	curMeteorCount = 1;
	leftarmrotatelimit = -1;
	rightarmrotatelimit = -1;
	leftarmrotate = true;
	rightarmrotate = true;
	rotatelefthand = 0;
	rotaterighthand = 0;
	moverobot = 0;
	moveleftleg = 0;
	moverightleg = 0;
	leftleglimit = 4;
	rightleglimit = 4;
	leftarmattack = 0;
	rightarmattack = 0;
	leftarmattacklimit = -4;
	rightarmattacklimit = -4;
	droidlimit = 3;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	collapse = 0;
	leftleg = true;
	rightleg = true;
	walk = true;
	die = false;
	//openleftgate = false;
	//openrightgate = false;
	leftgate = 0;
	rightgate = 0;
	repairShipPhase = 0;

	//robotleftattack = false;
	//robotrightattack = false;
	WepItf_Choices = Vector3(0, 0, 0);

	spawnPointN = Vector3(0, 0, 150);
	spawnPointS = Vector3(0, 0, -150); 
	spawnPointE = Vector3(-150, 0, 0);
	spawnPointW = Vector3(150, 0, 0);

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
	Crosshair = LoadTGA("Image//Crosshair.tga");
	UI_LoadingBG = LoadTGA("Image//UI_LoadScreen.tga");
	UI_LoadingSpinner = LoadTGA("Image//UI_LoadScreen.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

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

void SP2_Scene::InitWeaponModels()
{
	Normal_Sniper = LoadTGA("Image//Tex_Sniper.tga");
	Normal_Rifle = LoadTGA("Image//Tex_Rifle.tga");
	Normal_Shotgun = LoadTGA("Image//Tex_Shotgun.tga");
	meshList[GEO_PLAYERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//PlayerShip.obj");
	meshList[GEO_PLAYERSHIP]->textureID = LoadTGA("Image//Tex_PlayerShip.tga");

	// Damage
	Damage_Sniper = LoadTGA("Image//Tex_SniperDamage.tga");
	Damage_Rifle = LoadTGA("Image//Tex_RifleDamage.tga");
	Damage_Shotgun = LoadTGA("Image//Tex_ShotgunDamage.tga");

	// Ammo
	Capacity_Sniper = LoadTGA("Image//Tex_SniperAmmo.tga");
	Capacity_Rifle = LoadTGA("Image//Tex_RifleAmmo.tga");
	Capacity_Shotgun = LoadTGA("Image//Tex_ShotgunAmmo.tga");

	// Fast
	Fast_Sniper = LoadTGA("Image//Tex_SniperFast.tga");
	Fast_Rifle = LoadTGA("Image//Tex_RifleFast.tga");
	Fast_Shotgun = LoadTGA("Image//Tex_ShotgunFast.tga");

	meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1,1,1), 8, 8);

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

	//meshList[GEO_MOTHERSHIP] = MeshBuilder::GenerateOBJ("test", "OBJ//Mothership.obj");
	//meshList[GEO_MOTHERSHIP]->textureID = LoadTGA("Image//Tex_Mothership.tga");

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
	modelStack.Translate(140, 5, -10);
	modelStack.Rotate(200, 0, 1, 0);
	modelStack.Rotate(10, -1, 0, 0);
	modelStack.Scale(20, 20, 20);
	if (wepVal == 0)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 1, 1), 8, 8);
		RenderMesh(meshList[GEO_SMG], true);
	}
	else if (wepVal == 1)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 1, 1), 8, 8);
		meshList[GEO_RIFLE]->textureID = Normal_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 2)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 1, 1), 8, 8);
		meshList[GEO_SNIPER]->textureID = Normal_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 3)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 1, 1), 8, 8);
		meshList[GEO_SHOTGUN]->textureID = Normal_Shotgun;
		RenderMesh(meshList[GEO_SHOTGUN], true);
	}
	else if (wepVal == 4)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 0, 0), 8, 8);
		meshList[GEO_RIFLE]->textureID = Damage_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 5)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 0, 0), 8, 8);
		meshList[GEO_SNIPER]->textureID = Damage_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 6)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(1, 0, 0), 8, 8);
		meshList[GEO_SHOTGUN]->textureID = Damage_Shotgun;
		RenderMesh(meshList[GEO_SHOTGUN], true);
	}
	else if (wepVal == 7)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 0, 1), 8, 8);
		meshList[GEO_RIFLE]->textureID = Capacity_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 8)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 0, 1), 8, 8);
		meshList[GEO_SNIPER]->textureID = Capacity_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 9)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 0, 1), 8, 8);
		meshList[GEO_SHOTGUN]->textureID = Capacity_Shotgun;
		RenderMesh(meshList[GEO_SHOTGUN], true);
	}
	else if (wepVal == 10)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 1, 0), 8, 8);
		meshList[GEO_RIFLE]->textureID = Fast_Rifle;
		RenderMesh(meshList[GEO_RIFLE], true);
	}
	else if (wepVal == 11)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 1, 0), 8, 8);
		meshList[GEO_SNIPER]->textureID = Fast_Sniper;
		RenderMesh(meshList[GEO_SNIPER], true);
	}
	else if (wepVal == 12)
	{
		meshList[GEO_BULLET] = MeshBuilder::GenerateSphere("Bullet", Color(0, 1, 0), 8, 8);
		meshList[GEO_SHOTGUN]->textureID = Fast_Shotgun;
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

void SP2_Scene::GameState()
{
	if (basehp == 0 || playerhp == 0)
	{
		modelStack.PushMatrix();
		//translation here once map is out
		RenderMesh(meshList[GEO_GATE], true);
		modelStack.PopMatrix();
		//gatehp = 20;
		wave = 0;
		camera.Reset();
		wave = 1;
		playerhp = 100;
		basehp = 100;
	}
	if (RobotManager.CurrentRobotCount <= 0 && wave != 5 && wave != 6 && wave != 7)
	{
		weaponinterface = true;
	}
	else if (curMeteorCount <= 0)
	{
		weaponinterface = true;
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
		//for (int i = -2; i > -4; i--)
		//{
		//	for (int j = 0; j < 10; j++)
		//	{
		//		//range robot
		//		modelStack.PushMatrix();
		//		modelStack.Translate(10 * i + 20, 0.5, 10 * j + 140);
		//		modelStack.Translate(0, 0, -moverobot);
		//		RenderMesh(meshList[GEO_RANGEROBOTBODY], true);
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);1
		//		modelStack.Translate(0, 0, -10);
		//		modelStack.Translate(0, 0, 10);
		//		modelStack.Translate(0.3, 0, 0);
		//		RenderMesh(meshList[GEO_RANGEROBOTLEFTUPPERARM], true);
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//		RenderMesh(meshList[GEO_RANGEROBOTLEFTLOWERARM], true);
		//		modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//		modelStack.Translate(0, 0, -10);
		//		modelStack.Translate(0, 0, 10);
		//		modelStack.Translate(-0.3, 0, 0);
		//		RenderMesh(meshList[GEO_RANGEROBOTRIGHTUPPERARM], true);
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//		RenderMesh(meshList[GEO_RANGEROBOTRIGHTLOWERARM], true);
		//		modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		modelStack.Rotate(moveleftleg, 1, 0, 0);
		//		modelStack.Translate(0, 0, -10);
		//		modelStack.Translate(0, 0, 10);
		//		RenderMesh(meshList[GEO_RANGEROBOTLEFTLEG], true);
		//		modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		modelStack.Rotate(moverightleg, 1, 0, 0);
		//		RenderMesh(meshList[GEO_RANGEROBOTRIGHTLEG], true);
		//		modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//	}
		//}
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 2", Color(1, 0, 0), 3, 72.5, 87);

		if (RobotManager.CurrentRobotCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 2 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}
	if (wave == 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wave 3", Color(1, 0, 0), 3, 72.5, 87);

		if (curMeteorCount == 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wave 3 clear", Color(1, 0, 0), 3, 72.5, 45);
		}
	}
	if (wave == 4)
	{
		//for (int i = -2; i > -4; i--)
		//{
		//	for (int j = 0; j < 10; j++)
		//	{
		//		modelStack.PushMatrix();
		//		modelStack.Translate(10 * i + 20, 0.5, 10 * j + 140);
		//		modelStack.Translate(0, 0, -moverobot);
		//		RenderMesh(meshList[GEO_MIXEDROBOTBODY], true);
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);1
		//		modelStack.Translate(0, 0, -10);
		//		modelStack.Translate(0, 0, 10);
		//		modelStack.Translate(0.3, 0, 0);
		//		RenderMesh(meshList[GEO_MIXEDROBOTLEFTUPPERARM], true);
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//		RenderMesh(meshList[GEO_MIXEDROBOTLEFTLOWERARM], true);
		//		modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//		modelStack.Translate(0, 0, -10);
		//		modelStack.Translate(0, 0, 10);
		//		modelStack.Translate(-0.3, 0, 0);
		//		RenderMesh(meshList[GEO_MIXEDROBOTRIGHTUPPERARM], true);
		//		modelStack.PushMatrix();
		//		//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//		RenderMesh(meshList[GEO_MIXEDROBOTRIGHTLOWERARM], true);
		//		modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		modelStack.Rotate(moveleftleg, 1, 0, 0);
		//		modelStack.Translate(0, 0, -10);
		//		modelStack.Translate(0, 0, 10);
		//		RenderMesh(meshList[GEO_MIXEDROBOTLEFTLEG], true);
		//		modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		modelStack.Rotate(moverightleg, 1, 0, 0);
		//		RenderMesh(meshList[GEO_MIXEDROBOTRIGHTLEG], true);
		//		modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//	}
		//}
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
			repairShipPhase = true;
		}
	}
	if (wave == 6)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Final Stage", Color(1, 0, 0), 3, 72.5, 87);
		RenderTextOnScreen(meshList[GEO_TEXT], "Boss HP: " + std::to_string(bosshp), Color(0, 0.5, 0), 3, 72.5, 81);
		if (bosshp <= 0)
		{
			bosshp = 0;
			RenderImageOnScreen(UI_BG, 38, 5, 83, 76);
			RenderTextOnScreen(meshList[GEO_TEXT], "Victory Achieved", Color(0.000f, 0.788f, 0.820f), 4, 67.5, 76);
			//game won, go back to start screen
		}
	}
}

void SP2_Scene::RenderShip()
{
	if (repairShipPhase == 1 || repairShipPhase == 2)
	{
		modelStack.PushMatrix(); //Player ship
		modelStack.Translate(shipFallingX, shipFallingY, 75);
		modelStack.Rotate(45, 1, 0, 1.75);
		modelStack.Scale(4, 4, 4);
		RenderMesh(meshList[GEO_AXES], false);
		RenderMesh(meshList[GEO_PLAYERSHIP], true);
		modelStack.PopMatrix();
	}
	if (camera.shipCheck == true && shipFallingY <= 1 && repairShipPhase == 1)
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
		modelStack.Translate(shipFallingX, 7.5, 75);
		modelStack.Rotate(90, 0, 0, 1);
		modelStack.Scale(4, 4, 4);
		RenderMesh(meshList[GEO_AXES], false);
		RenderMesh(meshList[GEO_PLAYERSHIP], true);
		modelStack.PopMatrix();

		if (camera.shipCheck == true)
		{
			RenderImageOnScreen(UI_BG, 37.5, 2.75, 77.5, 25);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press <E> to pilot ship", Color(0.000f, 0.808f, 0.820f), 2.45, 62.5, 25);
		}
	}
}

void SP2_Scene::RobotAnimation(double dt)
{
	//left arm
	if (leftarmrotate == true)
	{
		rotatelefthand -= (float)(5 * dt);
	}
	else if (leftarmrotate == false)
	{
		rotatelefthand += (float)(5 * dt);
	}
	if (rotatelefthand <= leftarmrotatelimit)
	{
		leftarmrotate = false;
	}
	else if (rotatelefthand >= -leftarmrotatelimit) //+ (1 - leftarmrotatelimit))
	{
		leftarmrotate = true;
	}

	//right arm
	if (rightarmrotate == true)
	{
		rotaterighthand += (float)(5 * dt);
	}
	else if (rightarmrotate == false)
	{
		rotaterighthand -= (float)(5 * dt);
	}
	if (rotaterighthand <= rightarmrotatelimit)
	{
		rightarmrotate = true;
	}
	else if (rotaterighthand >= -rightarmrotatelimit) //+ (1 - rightarmrotatelimit))
	{
		rightarmrotate = false;
	}
	//left leg
	if (leftleg == true)
	{
		moveleftleg -= (float)(7 * dt);
	}
	else if (leftleg == false)
	{
		moveleftleg += (float)(7 * dt);
	}
	if (moveleftleg >= leftleglimit)
	{
		leftleg = true;
	}
	else if (moveleftleg <= -leftleglimit) //+ (0.5 - leftleglimit))
	{
		leftleg = false;
	}

	//right leg
	if (rightleg == true)
	{
		moverightleg += (float)(7 * dt);
	}
	else if (rightleg == false)
	{
		moverightleg -= (float)(7 * dt);
	}
	if (moverightleg >= rightleglimit)
	{
		rightleg = false;
	}
	else if (moverightleg <= -rightleglimit) //+ (0.5 - rightleglimit))
	{
		rightleg = true;
	}
	//walking
	if (walk == true)
	{
		moverobot += (float)(5 * dt);
		if (moverobot > 120)
		{
			walk = false;
		}
	}
	if (walk == false)
	{
		rotatelefthand = 0;
		rotaterighthand = 0;
		robotleftattack = true;
		robotrightattack = true;
	}
	//attack
	if (robotleftattack == true)
	{
		leftarmattack -= (float)(8 * dt);
	}
	else if (robotleftattack == false)
	{
		leftarmattack += (float)(8 * dt);
	}
	if (leftarmattack <= leftarmattacklimit)
	{
		robotleftattack = false;
	}
	else if (leftarmattack >= -leftarmattacklimit)
	{
		robotleftattack = true;
	}
	if (robotrightattack == true)
	{
		rightarmattack += (float)(8 * dt);
	}
	else if (robotrightattack == false)
	{
		rightarmattack -= (float)(8 * dt);
	}
	if (rightarmattack <= rightarmattacklimit)
	{
		robotrightattack = true;
	}
	else if (rightarmattack >= -rightarmattacklimit)
	{
		robotrightattack = false;
	}

	if (walk == false)
	{
		rotatelefthand = 0;
		rotaterighthand = 0;
	}
	if (robotleftattack == true)
	{
		leftarmattack -= (float)(8 * dt);
	}
	else if (robotleftattack == false)
	{
		leftarmattack += (float)(8 * dt);
	}
	if (leftarmattack >= leftarmattacklimit)
	{
		robotleftattack = true;
	}
	else if (leftarmattack <= -leftarmattacklimit)
	{
		robotleftattack = false;
	}
	if (robotrightattack == true)
	{
		rightarmattack += (float)(8 * dt);
	}
	else if (robotrightattack == false)
	{
		rightarmattack -= (float)(8 * dt);
	}
	if (rightarmattack >= rightarmattacklimit)
	{
		robotrightattack = false;
	}
	else if (rightarmattack <= -rightarmattacklimit)
	{
		robotrightattack = true;
	}

	//if (Robot::RobotHP == 0)
	//{
	//	die = true;
	//	RobotManager.CurrentRobotCount--;
	//}
	if (die == true)
	{
		collapse += (float)(15 * dt);
		if (collapse > 85)
		{
			die = false;
			collapse -= (float)(15 * dt);
		}
	}
	
	//droid repair animation
	if (repairgate == true)
	{
		droidrepair = true;
	}
	if (droidrepair == true)
	{
		droidrepairgate += (float)(5 * dt);
	}
	else if (droidrepair == false)
	{
		droidrepairgate -= (float)(5 * dt);
	}
	if (droidrepairgate >= droidlimit)
	{
		droidrepair = false;
	}
	else if (droidrepairgate <= -droidlimit)
	{
		droidrepair = true;
	}
		
}

void SP2_Scene::Update(double dt)
{
	camera.Update(dt);
	constRotation += (float)(10 * pause * dt);
	constTranslation += (float)(10 * pause  * dt);
	RobotAnimation(dt);
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
	translateX += (float)(10 * pause  * dt);
	if (GameLoading)
	{
		LoadTimer += dt;
		if (LoadTimer >= MaxLoadTime / 3 && !loadMap)
		{
			InitMapModels();
			loadMap = true;
		}
		else if (LoadTimer >= MaxLoadTime * 2 / 3 && !loadWep)
		{
			InitWeaponModels();
			loadWep = true;
		}
		else if (LoadTimer >= MaxLoadTime && !loadRobots)
		{
			InitRobots();
			loadRobots = true;
		}
		if (LoadTimer > MaxLoadTime && (Application::IsKeyPressed(VK_LBUTTON) || Application::IsKeyPressed(VK_RBUTTON)))
		{
			GameLoading = false;
		}
	}
	else
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
				/*buttonPress = false;
				buttonValue = 0;*/
				weaponinterface = false;
				playerhp--;
				repairShipPhase = 1;
			}
			else if (buttonPress == true && Application::IsKeyPressed('2'))
			{
				if (WepItf_Choices.y == 0){ weaponValue = 2; }
				else if (WepItf_Choices.y == 1){ weaponValue = 5; }
				else if (WepItf_Choices.y == 2){ weaponValue = 8; }
				else if (WepItf_Choices.y == 3){ weaponValue = 11; }
				WepSys.ClearList();
				wave += 1;
				/*buttonPress = false;
				buttonValue = 0;*/
				weaponinterface = false;
				basehp--;
				repairShipPhase = 2;
			}
			else if (buttonPress == true && Application::IsKeyPressed('3'))
			{
				if (WepItf_Choices.z == 0){ weaponValue = 3; }
				else if (WepItf_Choices.z == 1){ weaponValue = 6; }
				else if (WepItf_Choices.z == 2){ weaponValue = 9; }
				else if (WepItf_Choices.z == 3){ weaponValue = 12; }
				WepSys.ClearList();
				wave += 1;
				/*buttonPress = false;
				buttonValue = 0;*/
				weaponinterface = false;
				repairShipPhase = 3;
			}
			else if (buttonPress == true && Application::IsKeyPressed('4'))
			{
				WepSys.ClearList();
				wave += 1;
				/*buttonPress = false;
				buttonValue = 0;*/
				weaponinterface = false;
				repairShipPhase = 4;
			}
		}
		if (Application::IsKeyPressed('5'))
		{
			bosshp = 0;
			repairShipPhase = 0;
		}
		if (!weaponinterface)
		{
			randWepChoices = true;
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
		if (shipFallingY >= 1 && (repairShipPhase == 1 || repairShipPhase == 2))
		{
			shipFallingX += 0.1;
			shipFallingY -= 1;
		}
		if (camera.coreCheck == true && Application::IsKeyPressed('E'))
		{
			repairShipPhase = 2;
		}
		if (camera.shipCheck == true && Application::IsKeyPressed('E'))
		{
			repairShipPhase = 3;
		}

		framesPerSecond = 1 / dt;

		TownLightPosition.y += tweenVal / 15000;
		light[1].position.Set(TownLightPosition.x, TownLightPosition.y, TownLightPosition.z);
		RoomLightPosition.y += tweenVal / 150000;
		light[2].position.Set(RoomLightPosition.x, RoomLightPosition.y, RoomLightPosition.z);

		WepSys.SetStats(weaponValue);
		if (!CanFire)
		{
			RateOfFire = 0.2;
			GunWaitTime += pause * dt;
			if (GunWaitTime >= WepSys.RateOfFire)
			{
				CanFire = true;
			}
		}
		if (CanFire && weaponValue >= 0 && weaponValue <= 12 && Application::IsKeyPressed(VK_LBUTTON))
		{
			WepSys.BulletList.push_back(RayCast(WepSys.Damage, WepSys.Speed, camera.getCameraPosition(), camera.getLookVector()));
			CanFire = false;
			GunWaitTime = 0;
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

		if (CanFire && Application::IsKeyPressed(VK_RBUTTON))
		{
			RobotManager.RobotList.push_back(Robot(0, spawnPointN));
			RobotManager.RobotList.push_back(Robot(0, spawnPointE));
			RobotManager.RobotList.push_back(Robot(0, spawnPointS));
			RobotManager.RobotList.push_back(Robot(0, spawnPointW));
			CanFire = false;
			GunWaitTime = 0;
		}

		WepSys.IncrementPosition();
		RobotManager.IncrementPosition();
		for (std::list<Robot>::iterator iter = RobotManager.RobotList.begin(); iter != RobotManager.RobotList.end(); ++iter)
		{
			(*iter).BoundsCheck(WepSys);
		}
		RobotManager.CleanUp();
		WepSys.CleanUp();
	}
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
			meshList[GEO_SHOTGUN]->textureID = Normal_Shotgun;
			RenderMeshOnScreen(meshList[GEO_SHOTGUN], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.z == 1)
		{
			meshList[GEO_SHOTGUN]->textureID = Damage_Shotgun;
			RenderMeshOnScreen(meshList[GEO_SHOTGUN], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.z == 2)
		{
			meshList[GEO_SHOTGUN]->textureID = Capacity_Shotgun;
			RenderMeshOnScreen(meshList[GEO_SHOTGUN], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		else if (choices.z == 3)
		{
			meshList[GEO_SHOTGUN]->textureID = Fast_Shotgun;
			RenderMeshOnScreen(meshList[GEO_SHOTGUN], 1, 1, 125, 45, constRotation * pause * 5, Vector3(1, 1, 0));
		}
		RenderImageOnScreen(UI_BG, 30, 10, 120, 67.5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Gun: Shotgun", Color(0.000f, 0.808f, 0.820f), 4, 109, 70);
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
	RenderTextOnScreen(meshList[GEO_TEXT], fpsText.str(), Color(1, 1, 1), 2.5, 3, 7.5);
	std::stringstream coordText;
	coordText << std::fixed << std::setprecision(1) << "Player Location = (" << camera.getCameraPosition().x << ", " << camera.getCameraPosition().y << ", " << camera.getCameraPosition().z << ")";
	RenderTextOnScreen(meshList[GEO_TEXT], coordText.str(), Color(1, 1, 1), 2.5, 3, 4);
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

	RenderImageOnScreen(Crosshair, 10, 10, 80, 45);

	RenderWepScreen(weaponinterface, WepItf_Choices);

	RenderTeleporter(camera.teleCheck);

	//RenderGateText(camera.checkLeftGate || camera.checkRightGate || camera.checkFrontGate || camera.checkBackGate);

	RenderTextOnScreen(meshList[GEO_TEXT], "Base HP: " + std::to_string(basehp), Color(0, 0.5, 0), 3, 2.5, 87);

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
	modelStack.Translate(-leftgate, 0, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.frontGateB, 0.5, 17.2);
	modelStack.Translate(rightgate, 0, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderMesh(meshList[GEO_GATE], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SP2_Scene::RenderRocks()
{
	// Right
	modelStack.PushMatrix();
	modelStack.Translate(50, -4, 0);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(6, 6, 6);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(4, -4, -8);
	modelStack.Rotate(50, 1, 0, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(3, -4, 20);
	modelStack.Rotate(30, 1, 0, 0);
	modelStack.Scale(-3, -3, -3);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	// left
	modelStack.PushMatrix();
	modelStack.Translate(-50, -11, -8);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(6, 6, 6);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(-5, -4, -4);
	modelStack.Rotate(50, 0, 0, 1);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(-1, -4, -5);
	modelStack.Rotate(-20, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	// front
	modelStack.PushMatrix();
	modelStack.Translate(0, -2, -60);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(6, 6, 6);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(4, -4, -4);
	modelStack.Rotate(20, 0, 1, 0);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PopMatrix();
	modelStack.PopMatrix();

	// back
	modelStack.PushMatrix();
	modelStack.Translate(0, -4, 60);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(6, 6, 6);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(5, -4, 9);
	modelStack.Rotate(80, 1, 0, 0);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PushMatrix();
	modelStack.Translate(-8, -4, 0);
	RenderMesh(meshList[GEO_METEOR], true);

	modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PopMatrix();
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
			RenderImageOnScreen(Crosshair, 10, 10, 80 + (tweenVal / 4), 45);
		}
		else{
			RenderTextOnScreen(meshList[GEO_TEXT], "<Loading Completed>", Color(0, 0.5, 1), 6, 50, 45);
			RenderTextOnScreen(meshList[GEO_TEXT], "<Click Mouse To Start Game>", Color(0, 0.5, 1), 5, 45, 35);
		}
	}
	else{
		modelStack.PushMatrix();
		RenderMesh(meshList[GEO_AXES], false);
		modelStack.PopMatrix();

		readtextfile();
		GameState();

		//Render In-Hand Weapon
		modelStack.PushMatrix();
		RenderWeaponInHand(weaponValue, 5, 1, 1);
		modelStack.PopMatrix();

		for (auto i : WepSys.BulletList)
		{
			modelStack.PushMatrix();
			modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_BULLET], false);
			modelStack.PopMatrix();
		}

		for (auto i : RobotManager.RobotList)
		{
			modelStack.PushMatrix();
			modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
			modelStack.Rotate(i.rotateToTarget, 0, 1, 0);
			//HP
			modelStack.PushMatrix();
			modelStack.Translate(0, 7, 0);
			modelStack.Rotate(constRotation*2, 0, 1, 0);
			modelStack.PushMatrix();
			if (i.GetHealth() > 20)
			{
				modelStack.Scale(i.GetHealth() / 80, i.GetHealth() / 80, i.GetHealth() / 80);
				RenderMesh(meshList[GEO_ROBOTHEALTH], false);
			}
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Scale(i.GetHealth() / 100, i.GetHealth() / 100, i.GetHealth() / 100);
			RenderMesh(meshList[GEO_ROBOTHEALTH2], false);
			modelStack.PopMatrix();
			modelStack.PopMatrix();
			
			RenderMesh(meshList[GEO_RANGEROBOTBODY], true);
			modelStack.PushMatrix();
			//modelStack.Rotate(rotateAngle, 1, 0, 0);
			modelStack.Translate(0, 0, -10);
			modelStack.Translate(0, 0, 10);
			modelStack.Translate(0.3, 0, 0);
			RenderMesh(meshList[GEO_RANGEROBOTLEFTUPPERARM], true);
			modelStack.PushMatrix();
			//modelStack.Rotate(rotateAngle, 1, 0, 0);
			RenderMesh(meshList[GEO_RANGEROBOTLEFTLOWERARM], true);
			modelStack.PopMatrix();
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			//modelStack.Rotate(rotateAngle, 1, 0, 0);
			modelStack.Translate(0, 0, -10);
			modelStack.Translate(0, 0, 10);
			modelStack.Translate(-0.3, 0, 0);
			RenderMesh(meshList[GEO_RANGEROBOTRIGHTUPPERARM], true);
			modelStack.PushMatrix();
			//modelStack.Rotate(rotateAngle, 1, 0, 0);
			RenderMesh(meshList[GEO_RANGEROBOTRIGHTLOWERARM], true);
			modelStack.PopMatrix();
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Rotate(moveleftleg, 1, 0, 0);
			modelStack.Translate(0, 0, -10);
			modelStack.Translate(0, 0, 10);
			RenderMesh(meshList[GEO_RANGEROBOTLEFTLEG], true);
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Rotate(moverightleg, 1, 0, 0);
			RenderMesh(meshList[GEO_RANGEROBOTRIGHTLEG], true);
			modelStack.PopMatrix();
			modelStack.PopMatrix();
		}

		//for (auto i : numrobots.RobotList)
		//{
		//	//range robot
		//	modelStack.PushMatrix();
		//	i.Move();
		//	modelStack.Translate(i.Position().x, i.Position().y, i.Position().z);
		//	modelStack.Translate(0, 0, -moverobot);
		//	RenderMesh(meshList[GEO_RANGEROBOTBODY], true);
		//	modelStack.PushMatrix();
		//	//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//	modelStack.Translate(0, 0, -10);
		//	modelStack.Translate(0, 0, 10);
		//	modelStack.Translate(0.3, 0, 0);
		//	RenderMesh(meshList[GEO_RANGEROBOTLEFTUPPERARM], true);
		//	modelStack.PushMatrix();
		//	//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//	RenderMesh(meshList[GEO_RANGEROBOTLEFTLOWERARM], true);
		//	modelStack.PopMatrix();
		//	modelStack.PopMatrix();
		//	modelStack.PushMatrix();
		//	//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//	modelStack.Translate(0, 0, -10);
		//	modelStack.Translate(0, 0, 10);
		//	modelStack.Translate(-0.3, 0, 0);
		//	RenderMesh(meshList[GEO_RANGEROBOTRIGHTUPPERARM], true);
		//	modelStack.PushMatrix();
		//	//modelStack.Rotate(rotateAngle, 1, 0, 0);
		//	RenderMesh(meshList[GEO_RANGEROBOTRIGHTLOWERARM], true);
		//	modelStack.PopMatrix();
		//	modelStack.PopMatrix();
		//	modelStack.PushMatrix();
		//	modelStack.Rotate(moveleftleg, 1, 0, 0);
		//	modelStack.Translate(0, 0, -10);
		//	modelStack.Translate(0, 0, 10);
		//	RenderMesh(meshList[GEO_RANGEROBOTLEFTLEG], true);
		//	modelStack.PopMatrix();
		//	modelStack.PushMatrix();
		//	modelStack.Rotate(moverightleg, 1, 0, 0);
		//	RenderMesh(meshList[GEO_RANGEROBOTRIGHTLEG], true);
		//	modelStack.PopMatrix();
		//	modelStack.PopMatrix();
		//}

		////drone
		//modelStack.PushMatrix();
		//modelStack.Translate(0, 0, -12);
		//modelStack.Rotate(180, 0, 1, 0);
		//RenderMesh(meshList[GEO_DRONEBODY], true);
		//	modelStack.PushMatrix();
		//	modelStack.Rotate(droidrepairgate, 0, 0, 1);
		//	modelStack.Translate(0, 0, -12);
		//	modelStack.Translate(0, 0, 12);
		//	RenderMesh(meshList[GEO_DRONELEFTUPPERARM], true);
		//		modelStack.PushMatrix();
		//		RenderMesh(meshList[GEO_DRONELEFTLOWERARM], true);
		//		modelStack.PopMatrix();
		//	modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		modelStack.Rotate(droidrepairgate, 0, 0, 1);
		//		modelStack.Translate(0, 0, -12);
		//		modelStack.Translate(0, 0, 12);
		//		RenderMesh(meshList[GEO_DRONERIGHTUPPERARM], true);
		//			modelStack.PushMatrix();
		//			RenderMesh(meshList[GEO_DRONERIGHTLOWERARM], true);
		//			modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//modelStack.PopMatrix();

		////mixed robot
		//modelStack.PushMatrix();
		//modelStack.Translate(-10, 0, 0);
		//modelStack.Rotate(-90, 0, 1, 0);
		//RenderMesh(meshList[GEO_MIXEDROBOTBODY], true);
		//	modelStack.PushMatrix();
		//	RenderMesh(meshList[GEO_MIXEDROBOTLEFTARM], true);
		//	modelStack.PopMatrix();
		//		modelStack.PushMatrix();
		//		RenderMesh(meshList[GEO_MIXEDROBOTRIGHTARM], true);
		//		modelStack.PopMatrix();
		//			modelStack.PushMatrix();
		//			RenderMesh(meshList[GEO_MIXEDROBOTLEFTLEG], true);
		//			modelStack.PopMatrix();
		//				modelStack.PushMatrix();
		//				RenderMesh(meshList[GEO_MIXEDROBOTRIGHTLEG], true);
		//				modelStack.PopMatrix();
		//modelStack.PopMatrix();
		//modelStack.PopMatrix();*/

		/*modelStack.PushMatrix();
		modelStack.Translate(17.2, 0.5, -2.15);
		modelStack.Translate(0, 0, -leftgate);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(1.55, 1.55, 2.5);
		RenderGate(true);
=======
	////mixed robot
	//modelStack.PushMatrix();
	//modelStack.Translate(-10, 0, 0);
	//modelStack.Rotate(-90, 0, 1, 0);
	//RenderMesh(meshList[GEO_MIXEDROBOTBODY], true);
	//	modelStack.PushMatrix();
	//	RenderMesh(meshList[GEO_MIXEDROBOTLEFTARM], true);
	//	modelStack.PopMatrix();
	//		modelStack.PushMatrix();
	//		RenderMesh(meshList[GEO_MIXEDROBOTRIGHTARM], true);
	//		modelStack.PopMatrix();
	//			modelStack.PushMatrix();
	//			RenderMesh(meshList[GEO_MIXEDROBOTLEFTLEG], true);
	//			modelStack.PopMatrix();
	//				modelStack.PushMatrix();
	//				RenderMesh(meshList[GEO_MIXEDROBOTRIGHTLEG], true);
	//				modelStack.PopMatrix();
	//modelStack.PopMatrix();
	//modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	/*modelStack.PushMatrix();
>>>>>>> 70778617242d0c6ed5e730eeb07ea6243176d5a7
	modelStack.Translate(17.2, 0.5, -2.15);
	modelStack.Translate(0, 0, -leftgate);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1.55, 1.55, 2.5);
	RenderGate(true);
>>>>>>> a6da9c29979dade4382dc44a0de55bfaa8b24271
		modelStack.PushMatrix();
		modelStack.Translate(-3, 0, 0);
		modelStack.Translate(rightgate, 0, 0);
		RenderGate(true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-17.2, 0.5, -2.15);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(1.55, 1.55, 2.5);
		RenderGate(true);
		modelStack.PushMatrix();
		modelStack.Translate(-3, 0, 0);
		RenderGate(true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-2.15, 0.5, -17.2);
		modelStack.Scale(1.55, 1.55, 2.5);
		RenderGate(true);
		modelStack.PushMatrix();
		modelStack.Translate(3, 0, 0);
		RenderGate(true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-2.15, 0.5, 17.2);
		modelStack.Scale(1.55, 1.55, 2.5);
		RenderGate(true);
		modelStack.PushMatrix();
		modelStack.Translate(3, 0, 0);
		RenderGate(true);
		modelStack.PopMatrix();
		modelStack.PopMatrix();*/
	modelStack.PopMatrix();

		RenderRocks();

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

		RenderGate();
		RenderLevel();
		RenderShip();
		//DO NOT RENDER ANYTHING UNDER THIS//

		RenderUI();
	}
}

void SP2_Scene::Exit()
{
	//engine->drop();
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

