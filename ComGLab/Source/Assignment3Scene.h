#ifndef Assignment3Scene_H
#define Assignment3Scene_H

#include "Scene.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera3.h"
#include "MatrixStack.h"
#include "Material.h"
#include "LoadTGA.h"
#include <iomanip>
#include <string>

#include <vector>
using std::vector;

#include <sstream>

#include "irrklang.h"
using namespace irrklang;
#pragma comment(lib, "../irrKlang-1.5.0/lib/Win32-visualStudio/irrKlang.lib")

extern int S_Width, S_Height;


class Assignment3Scene : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_QUAD,
		GEO_CUBE,
		GEO_CIRCLE,
		GEO_RING,
		GEO_SPHERE,
		GEO_HEMISPHERE,
		GEO_TORUS,
		GEO_CYLINDER,
		GEO_CONE,

		//Skybox
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		//Custom Models
		GEO_PLANK,
		GEO_OBJ_1,
		GEO_OBJ_2,
		GEO_OBJ_3,
		GEO_OBJ_4,
		GEO_OBJ_5,
		GEO_OBJ_6,
		GEO_OBJ_7,
		GEO_OBJ_8,
		GEO_OBJ_9,
		GEO_OBJ_10,
		GEO_OBJ_11,
		GEO_PLAYER_HOME,
		GEO_HOME_TABLE,
		GEO_HOME_CHAIR,
		GEO_LIGHTPILLAR,
		GEO_SNIPER,

		//Kyogre Meshes
		//Blue Meshes
		GEO_B_SPHERE,
		GEO_B_CONE,
		GEO_B_CUBE,
		GEO_B_PRISM,
		GEO_B_HEMISPHERE,
		GEO_B_QUARTERSPHERE,

		//White Meshes
		GEO_W_SPHERE,
		GEO_W_CUBE,
		GEO_W_TRAPEZIUM,
		GEO_W_QUARTERSPHERE,
		GEO_W_HEMISPHERE,
		GEO_W_CYLINDER,

		//Red Meshes
		GEO_R_CUBE,
		GEO_R_SPHERE,
		GEO_R_CYLINDER,
		GEO_R_TORUS,
		GEO_R_QUARTERSPHERE,
		GEO_R_HALFTORUS,
		GEO_R_HALFTORUSTHICK,

		//Others
		GEO_BLK_SPHERE,
		GEO_Y_TORUS,
		GEO_LB_THINTORUS,
		GEO_P_THINTORUS,

		//add these enum in GEOMETRY_TYPE before NUM_GEOMETRY
		GEO_TEXT,

		GEO_LIGHTBALL,
		GEO_FLOOR,

		NUM_GEOMETRY,
	};

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

		//add these enum in UNIFORM_TYPE before U_TOTAL
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		//For Light 0
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		//For Light 1
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		//For Light 2
		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_TYPE,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,

		U_NUMLIGHTS,
		U_TOTAL,
	};

public:
	Assignment3Scene();
	~Assignment3Scene();

	Light light[3];
	void RenderMesh(Mesh *mesh, bool enableLight);

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	float tweenVal;
	float constRotation;
	float translateX;
	float scaleAll;
	float rotationalLimit;
	float translationLimit;
	float scalingLimit;
	bool rLimiter;
	bool toggleLimiters;
	bool limitersON;
	float kyogreTranslation;
	float DoorRot;
	bool canUseDoor = true;

	unsigned short skyboxID = 0;

	Position lightDefaultPos;

	bool lightOff;

	Camera3 camera;

	MS modelStack, viewStack, projectionStack;

private:
	GLuint SB_Day_front, SB_Day_back, SB_Day_top, SB_Day_bottom, SB_Day_left, SB_Day_right;
	GLuint SB_Nite_front, SB_Nite_back, SB_Nite_top, SB_Nite_bottom, SB_Nite_left, SB_Nite_right;
	void RenderSkybox(Vector3 Position);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void initLights();


	//Render Calls
	void RenderRails();
	void RenderTownBase();
	void RenderBridges();
	void RenderTrees();
	void RenderInteractiveParts();
	void RenderTownHouses();
	void RenderText();

	void getTreeCoords();
	vector<float> treeCoords;

	void initKyogreMeshes();
	void renderKyogre(float x, float y, float z);

	//Interaction Values
	bool InteractionBoundsCheck(Vector3 CameraPosition, int value);
	bool canPlacePlank = false;
	bool plankPlaced = false;
	Boundary PlankIxB;
	bool canInteractWithKyogre = false;
	bool interactedWithKyogre = false;
	Boundary KyogreIxB;
	void Assignment3Scene::initBounds();
	float framesPerSecond;
	Boundary DoorIxB;
	bool canOpenDoor = false;
	bool DoorOpened = false;

	Boundary LampIxB;
	bool canUseLamp = false;
	bool IsNight = false;
	bool LampActive = true;
	float timeCheck = 0;

	//Light Stuff
	Vector3 TownLightPosition;
	Vector3 RoomLightPosition;
};



#endif