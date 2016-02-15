#include "Assignment3Scene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"
#include "Application.h"
#include "MeshBuilder.h"
#include "Utility.h"

bool plankBoundActive;
bool doorBoundActive;

ISoundEngine* engine;

const std::string SoundName[] = {
	"Sounds//Spacebattle.mp3"
};

ISound* Song;

Assignment3Scene::Assignment3Scene()
{
}

Assignment3Scene::~Assignment3Scene()
{
}

void Assignment3Scene::Init()
{
	engine = createIrrKlangDevice();
	engine->addSoundSourceFromFile(SoundName[0].c_str());
	Song = engine->play2D(SoundName[0].c_str(), true, false, true);
	Song->setVolume(0.25);

	// Init VBO here
	glClearColor(0.15f, 0.2f, 0.35f, 0.0f);

	Mtx44 projection;
	projection.SetToPerspective(45.0f, static_cast < float >(S_Width) / static_cast < float >(S_Height), 0.1f, 3000.0f);
	projectionStack.LoadMatrix(projection);

	camera.Init(Vector3(0, 4, 8.5), Vector3(0, 4, 0), Vector3(0, 1, 0));

	//Initiallising Variables For Translate, Rotate, Scale
	tweenVal = 0;
	constRotation = 0;
	kyogreTranslation = 0;
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

	meshList[GEO_SNIPER] = MeshBuilder::GenerateOBJ("test", "OBJ//Sniper.obj");
	meshList[GEO_SNIPER]->textureID = LoadTGA("Image//Tex_Sniper.tga");

	initBounds();
}

void Assignment3Scene::RenderText(Mesh* mesh, std::string text, Color color)
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

void Assignment3Scene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

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
	//Add these code just before glEnable(GL_DEPTH_TEST);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}


void Assignment3Scene::RenderMesh(Mesh *mesh, bool enableLight)
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

void Assignment3Scene::initLights()
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

	/*////Light 1 - Town Light
	//m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	//m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	//m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	//m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	//m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	//m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	//m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	//m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	//m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	//m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	//m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	//TownLightPosition.Set(0, 3, -60);

	//light[1].type = Light::LIGHT_SPOT;
	//light[1].position.Set(0, 0, 0);
	//light[1].color.Set(0,0, 0);
	//light[1].power = 0;
	//light[1].kC = 1.f;
	//light[1].kL = 0.01f;
	//light[1].kQ = 0.001f;
	//light[1].cosCutoff = cos(Math::DegreeToRadian(180));
	//light[1].cosInner = cos(Math::DegreeToRadian(30));
	//light[1].exponent = 3.0f;
	//light[1].spotDirection.Set(0.0f, 1.0f, 0.0f);

	//glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	//glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	//glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	//glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	//glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	//glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	//glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	//glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	//glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	////Light 3 - Room Light
	//m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
	//m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
	//m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
	//m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
	//m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
	//m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
	//m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
	//m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
	//m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
	//m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
	//m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

	//RoomLightPosition.Set(-4.8, 2.75, -133.8);

	//light[2].type = Light::LIGHT_POINT;
	//light[2].position.Set(RoomLightPosition.x, RoomLightPosition.y, RoomLightPosition.z);
	//light[2].color.Set(1, 0.95, 1);
	//light[2].power = 1;
	//light[2].kC = 1.f;
	//light[2].kL = 0.01f;
	//light[2].kQ = 0.001f;
	//light[2].cosCutoff = cos(Math::DegreeToRadian(45));
	//light[2].cosInner = cos(Math::DegreeToRadian(30));
	//light[2].exponent = 3.0f;
	//light[2].spotDirection.Set(0.0f, 1.0f, 0.0f);

	//glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
	//glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
	//glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
	//glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
	//glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
	//glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
	//glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], light[2].cosCutoff);
	//glUniform1f(m_parameters[U_LIGHT2_COSINNER], light[2].cosInner);
	//glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);*/
}

void Assignment3Scene::initBounds()
{
	/*PlankIxB.set(-5.0f, 5.0f, -1.0f, 8.0f);
	PlankIxB.type = 1;
	KyogreIxB.set(63.0f, 72.0f, -73.0f, -64.0f);
	KyogreIxB.type = 1;
	DoorIxB.set(1, 5, -125, -116);
	DoorIxB.type = 1;
	LampIxB.set(-8, 1, -136.125, -126);
	LampIxB.type = 1;*/
}

void Assignment3Scene::Update(double dt)
{
	camera.Update(dt);
	plankBoundActive = plankPlaced;
	doorBoundActive = DoorOpened;
	////Updating bound data
	//InteractionBoundsCheck(camera.getCameraPosition(), 1);
	//canPlacePlank = PlankIxB.isWithin;
	//InteractionBoundsCheck(camera.getCameraPosition(), 2);
	//canInteractWithKyogre = KyogreIxB.isWithin;
	//InteractionBoundsCheck(camera.getCameraPosition(), 3);
	//canUseLamp = LampIxB.isWithin;
	//InteractionBoundsCheck(camera.getCameraPosition(), 4);
	//canOpenDoor = DoorIxB.isWithin;
	////
	//if (interactedWithKyogre && kyogreTranslation < 30)
	//{
	//	kyogreTranslation += (float)(10 * dt);
	//}
	//if (DoorOpened && DoorRot <= 120)
	//{
	//	canUseDoor = false;
	//	DoorRot += (float)(120 * dt);
	//}
	//if (DoorOpened == false && DoorRot >= 0)
	//{
	//	canUseDoor = false;
	//	DoorRot += (float)(-120 * dt);
	//}
	//if (DoorRot <= 0 || DoorRot >= 120)
	//{
	//	canUseDoor = true;
	//}
	constRotation += (float)(10 * dt);
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
	translateX += (float)(10 * dt /* EXTRA */ * 2);
	//End

	//Resetting Scaling
	if (scaleAll >= scalingLimit)
	{
		scaleAll = 5;
	}
	scaleAll += (float)(2 * dt);
	/*if (Application::IsKeyPressed(VK_SPACE) || Application::IsKeyPressed(VK_LBUTTON))
	{
		
		if (canPlacePlank == true && plankPlaced == false)
		{
			plankPlaced = true;
		}
		if (canInteractWithKyogre == true && interactedWithKyogre == false)
		{
			interactedWithKyogre = true;
		}
		if (canOpenDoor == true && DoorOpened == false && canUseDoor == true)
		{
			DoorOpened = true;
		}
		else if (canOpenDoor == true && DoorOpened == true && canUseDoor == true)
		{
			DoorOpened = false;
		}
		if (canUseLamp == true && IsNight == false && LampActive == true)
		{
			LampActive = false;
			IsNight = true;
			timeCheck = 2;
		}
		else if (canUseLamp == true && IsNight == true && LampActive == true)
		{
			LampActive = false;
			IsNight = false;
			timeCheck = 2;
		}
	}*/
	/*if (timeCheck <= 0)
	{
		LampActive = true;
	}
	else timeCheck -= dt;*/
	if (Application::IsKeyPressed('1'))
	{
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('2'))
	{
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('3'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (Application::IsKeyPressed('4'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	framesPerSecond = 1 / dt;

	TownLightPosition.y += tweenVal / 15000;
	light[1].position.Set(TownLightPosition.x, TownLightPosition.y, TownLightPosition.z);
	RoomLightPosition.y += tweenVal / 150000;
	light[2].position.Set(RoomLightPosition.x, RoomLightPosition.y, RoomLightPosition.z);

	/*if (IsNight)
	{
		light[0].color.Set(0, 0, 0);
		light[0].power = 0;
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
		light[1].color.Set(0.6, 0.9, 0.9);
		light[1].power = 1;
		glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		light[2].color.Set(0.6, 0.9, 0.9);
		light[2].power = 1;
		glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
		glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);

		meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
		meshList[GEO_FRONT]->textureID = SB_Nite_front;
		meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
		meshList[GEO_BACK]->textureID = SB_Nite_back;
		meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
		meshList[GEO_TOP]->textureID = SB_Nite_top;
		meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
		meshList[GEO_BOTTOM]->textureID = SB_Nite_bottom;
		meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
		meshList[GEO_LEFT]->textureID = SB_Nite_left;
		meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
		meshList[GEO_RIGHT]->textureID = SB_Nite_right;
	}
	else if (IsNight == false)
	{
		light[0].color.Set(1, 0.95, 1);
		light[0].power = 1;
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
		light[1].color.Set(0, 0, 0);
		light[1].power = 0;
		glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		light[2].color.Set(0, 0, 0);
		light[2].power = 0;
		glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
		glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);

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
	}*/
}

void Assignment3Scene::RenderSkybox(Vector3 Position)
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

void Assignment3Scene::Render()
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

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0,2,5);
	modelStack.Scale(0.8, 0.8, 0.8);
	RenderMesh(meshList[GEO_SNIPER], true);
	modelStack.PopMatrix();
}

void Assignment3Scene::Exit()
{
	engine->drop();
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

