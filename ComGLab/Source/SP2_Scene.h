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
		GEO_ENEMYBULLET,

		//add these enum in GEOMETRY_TYPE before NUM_GEOMETRY
		GEO_TEXT,

		GEO_LIGHTBALL,
		GEO_FLOOR,

		GEO_BASE,
		GEO_GATE_SIDE,
		GEO_POWERUP,
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
	float ShipX;
	float ShipY;
	float ShipRot;
	unsigned short repairShipPhase;
	bool walk;
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
	bool canUseDoor = true;
	int weaponSelect;
	bool sniper, rifle;

	unsigned short skyboxID = 0;

	Position lightDefaultPos;

	bool lightOff;

	Camera3 camera;

	MS modelStack, viewStack, projectionStack;

	//Mothership Stuff
	WeaponSystem EnemyWepSys;
	RobotManager MothershipHandler;
	bool Intro = false;
	bool weaponsOn = false;
	bool ShipSpawned = false;
	Vector3 ShipPos1 = Vector3(0, 10, 700);
	Vector3 ShipPos2 = Vector3(-60, 40, 700);
	Vector3 ShipPos3 = Vector3(60, 40, 700); 
	float ChoiceTimer = 0;
	float ShotTimer = 0;
	float ShipRotation = 90;
	Boundary PlayerShipBounds;
	Vector3 PlayerShipCoord;
	void MothershipBulletSpawner(float BulletDamage, float BulletSpeed, int choice, Vector3 position);

	WeaponSystem WepSys;
	RobotManager RobotManager;

	//Gun Stuff
	bool CanFire = true;
	float GunWaitTime = 0;
	int CurrentAmmo = 0;
	bool IsReloading = false;
	float ReloadWaitTime = 0;
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

	bool GameOver = false;

	void InitKeyVariables();

private:
	unsigned short weaponValue;
	GLuint SB_Day_front, SB_Day_back, SB_Day_top, SB_Day_bottom, SB_Day_left, SB_Day_right;
	GLuint SB_Nite_front, SB_Nite_back, SB_Nite_top, SB_Nite_bottom, SB_Nite_left, SB_Nite_right;
	GLuint Normal_Sniper, Normal_Rifle, Normal_SMG;
	GLuint Damage_Sniper, Damage_Rifle, Damage_SMG;
	GLuint Capacity_Sniper, Capacity_Rifle, Capacity_SMG;
	GLuint Fast_Sniper, Fast_Rifle, Fast_SMG;
	GLuint Crosshair, Crosshair2;
	GLuint UI_BG, UI_HP_Red, UI_HP_Green, UI_WepSel_BG, UI_LoadingBG, UI_LoadingBarOverlay;
	GLuint UI_Logo;
	GLuint Bullet_Red, Bullet_Teal, Bullet_Blue, Bullet_Green, Bullet_White;
	
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

	//Render Calls
	void readtextfile();
	void GameState();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size = 1, float x = 0, float y = 0);
	void RenderWeaponInHand(unsigned short wepVal = 0, float size = 1, float x = 0, float y = 0);
	void RenderImageInMap(GLuint texture, float Xsize = 1, float Ysize = 1);
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
	//void RobotAnimation(double dt);
	void RenderShip();
	void RenderSpaceMap();
	void RenderBase();
	void RenderRobots();
	void RenderPowerUp(unsigned short PowerUpType = 1);

	//Interaction Values
	bool buttonPress;
	float buttonValue;

	//test on screen values
	double fps;
	unsigned int basehp;
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
	float timer;
	bool weaponinterface;
	bool SpawnedRobots = false;
	bool PlayBGM = false;
	bool beforeWave7 = false;


	//Light Stuff
	Vector3 TownLightPosition;
	Vector3 RoomLightPosition;

	// Base Stuff
	Vector3 basePosition;

	//Main Menu Stuff
	Vector3 UICrystalPosition = Vector3(2, 0, 0);
	unsigned short UICrystalChoice;
	bool InMainMenu = false;
	bool GameStarted = false;
	bool InSettings = false;

	//Load Values
	float LoadTimer = 0;
	float MaxLoadTime = 1;
	bool GameLoading = true;
	bool loadRobots = false;
	bool loadMap = false;
	bool loadWep = false;

	//mothership defeated animation
	float explosion;

	//Power Up Stuff
	bool PowerUpActive = true;
	unsigned short PowerUpVal = 0;
};


#endif