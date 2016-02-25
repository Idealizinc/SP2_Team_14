#ifndef SP2_SCENE_H
#define SP2_SCENE_H

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
#include "WeaponSystem.h"
#include "Robot.h"
#include "RobotManager.h"

using std::vector;

#include <sstream>

#include "irrklang.h"
using namespace irrklang;
#pragma comment(lib, "../irrKlang-1.5.0/lib/Win32-visualStudio/irrKlang.lib")

extern int S_Width, S_Height;


class SP2_Scene : public Scene
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

		//Weapons
		GEO_SMG,
		GEO_SNIPER,
		GEO_RIFLE,
		GEO_SHOTGUN,

		//Ships
		GEO_PLAYERSHIP,
		GEO_MOTHERSHIP,

		//drone
		GEO_DRONEBODY,
		GEO_DRONELEFTUPPERARM,
		GEO_DRONELEFTLOWERARM,
		GEO_DRONERIGHTUPPERARM,
		GEO_DRONERIGHTLOWERARM,

		//robots
		GEO_ROBOTHEALTH,
		GEO_ROBOTHEALTH2,

		GEO_MELEEROBOTBODY,
		GEO_MELEEROBOTLEFTUPPERARM,
		GEO_MELEEROBOTLEFTLOWERARM,
		GEO_MELEEROBOTRIGHTUPPERARM,
		GEO_MELEEROBOTRIGHTLOWERARM,
		GEO_MELEEROBOTRIGHTARM,
		GEO_MELEEROBOTLEFTLEG,
		GEO_MELEEROBOTRIGHTLEG,

		GEO_RANGEROBOTBODY,
		GEO_RANGEROBOTLEFTUPPERARM,
		GEO_RANGEROBOTLEFTLOWERARM,
		GEO_RANGEROBOTRIGHTUPPERARM,
		GEO_RANGEROBOTRIGHTLOWERARM,
		GEO_RANGEROBOTLEFTLEG,
		GEO_RANGEROBOTRIGHTLEG,

		GEO_MIXEDROBOTBODY,
		GEO_MIXEDROBOTLEFTUPPERARM,
		GEO_MIXEDROBOTLEFTLOWERARM,
		GEO_MIXEDROBOTRIGHTUPPERARM,
		GEO_MIXEDROBOTRIGHTLOWERARM,
		GEO_MIXEDROBOTLEFTLEG,
		GEO_MIXEDROBOTRIGHTLEG,

		// UNCLASSIFIED
		GEO_GATE,
		GEO_METEOR,
		//GEO_COMPUTER,
		GEO_CRYSTAL,
		GEO_CRYSTALBASE,
		GEO_MOONFLOOR,
		GEO_TELEPORTER,
		GEO_BULLET,

		//add these enum in GEOMETRY_TYPE before NUM_GEOMETRY
		GEO_TEXT,

		GEO_LIGHTBALL,
		GEO_FLOOR,

		GEO_BASE,
		GEO_GATE_SIDE,
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
	SP2_Scene();
	~SP2_Scene();

	Light light[3];
	void RenderMesh(Mesh *mesh, bool enableLight);

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render(double dt);
	virtual void Exit();
	float tweenVal;
	float constRotation;
	float translateX;
	float scaleAll;
	float rotationalLimit;
	float translationLimit;
	float scalingLimit;
	float shipFallingX;
	float shipFallingY;
	unsigned short repairShipPhase;

	unsigned int curRobotCount;
	//unsigned int curMeteorCount;
	//robot animation
	bool walk;
	float moverobot;
	float rotatelefthand;
	float rotaterighthand;
	float leftarmrotatelimit;
	float rightarmrotatelimit;
	bool leftarmrotate;
	bool rightarmrotate;

	float leftleglimit;
	float rightleglimit;
	float moveleftleg;
	float moverightleg;
	bool leftleg;
	bool rightleg;

	bool robotleftattack;
	bool robotrightattack;
	float rightarmattack;
	float rightarmattacklimit;
	float leftarmattack;
	float leftarmattacklimit;

	float collapse;
	bool die;

	bool droidrepair;
	float droidrepairgate;
	float droidlimit;
	//end of robot
	//mothership
	bool moveMship;
	float moveMshipUp;
	float rotateMship;
	bool floatRocks;
	float moveRocks; //for space map wave 6float droidrepairgate;

	bool rLimiter;
	bool toggleLimiters;
	bool limitersON;
	float constTranslation;
	float DoorRot;
	bool canUseDoor = true;
	float leftgate;
	float rightgate;
	bool openleftgate;
	bool openrightgate;
	int weaponSelect;
	bool sniper, rifle;
	//unsigned short curRobotCount;
	//unsigned int pause;

	unsigned short skyboxID = 0;

	Position lightDefaultPos;

	bool lightOff;

	Camera3 camera;

	MS modelStack, viewStack, projectionStack;

	WeaponSystem WepSys;
	RobotManager RobotManager;

	//Gun Stuff
	bool CanFire = true;
	float GunWaitTime = 0;
	enum Gun_Type
	{
		N_SMG,
		N_Rifle,
		N_Sniper,
		N_Shotgun,
		D_Rifle,
		D_Sniper,
		D_Shotgun,
		C_Rifle,
		C_Sniper,
		C_Shotgun,
		F_Rifle,
		F_Sniper,
		F_Shotgun,
	};
	Gun_Type currentGun = N_SMG;
	float RateOfFire;
	float Damage = 0;
	float MaxAmmo = 0;
	float CurrAmmo = MaxAmmo;

private:
	unsigned short weaponValue;
	GLuint SB_Day_front, SB_Day_back, SB_Day_top, SB_Day_bottom, SB_Day_left, SB_Day_right;
	GLuint SB_Nite_front, SB_Nite_back, SB_Nite_top, SB_Nite_bottom, SB_Nite_left, SB_Nite_right;
	GLuint Normal_Sniper, Normal_Rifle, Normal_Shotgun;
	GLuint Damage_Sniper, Damage_Rifle, Damage_Shotgun;
	GLuint Capacity_Sniper, Capacity_Rifle, Capacity_Shotgun;
	GLuint Fast_Sniper, Fast_Rifle, Fast_Shotgun;
	GLuint Crosshair;
	GLuint UI_BG, UI_HP_Red, UI_HP_Green, UI_WepSel_BG, UI_LoadingBG, UI_LoadingSpinner;
	void RenderSkybox(Vector3 Position);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];

	//INIT Calls
	void initLights();
	void InitWeaponModels();
	void InitRobots();
	void InitMapModels();
	void initUIElements();

	//Load Values
	float LoadTimer = 0;
	float MaxLoadTime = 1;
	bool GameLoading = true;
	bool loadRobots = false;
	bool loadMap = false;
	bool loadWep = false;

	//Render Calls
	void readtextfile();
	void GameState();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size = 1, float x = 0, float y = 0);
	void RenderWeaponInHand(unsigned short wepVal = 0, float size = 1, float x = 0, float y = 0);
	void RenderImageOnScreen(GLuint texture, float Xsize = 1, float Ysize = 1, float Xpos = 0, float Ypos = 0);
	void RenderMeshOnScreen(Mesh* mesh, float Xsize = 1, float Ysize = 1, float Xpos = 0, float Ypos = 0, float Angle = 0, Vector3 RotationDir = Vector3(0, 0, 0));
	void RenderWepScreen(bool render = false, Vector3 choices = Vector3(0, 0, 0));
	void RenderTeleporter(bool render = false);
	void RenderGateText(bool render = false);
	void RenderRocks();
	void RenderLevel();
	void initBounds();
	void RenderGate(bool render = false);
	void RenderUI();
	void RobotAnimation(double dt);
	void RenderShip();
	void RenderSpaceMap();
	void RenderBase();
	void RenderRobots();

	//Interaction Values
	bool buttonPress;
	int buttonValue;

	//test on screen values
	double fps;
	unsigned int basehp;
	unsigned int gatehp;
	unsigned int bosshp;
	unsigned int playerhp;
	unsigned int ammo;
	unsigned int wave;
	float framesPerSecond;

	// Weapon Interface Stuff
	Vector3 WepItf_Choices;
	bool randWepChoices = true;

	//Robot Spawn
	Vector3 spawnPointN;
	Vector3 spawnPointS;
	Vector3 spawnPointE;
	Vector3 spawnPointW;

	//check game state
	int state;
	float timer;
	bool weaponinterface;
	bool repairgate;

	//Light Stuff
	Vector3 TownLightPosition;
	Vector3 RoomLightPosition;

	// Base Stuff
	Vector3 basePosition;
};


#endif