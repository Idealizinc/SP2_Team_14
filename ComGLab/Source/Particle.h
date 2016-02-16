#ifndef PARTICLE_H
#define PARTICLE_H

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


class Particle : public Scene
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
		GEO_SNIPER,

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
	Particle();
	~Particle();

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
	bool rLimiter;
	bool toggleLimiters;
	bool limitersON;
	float constTranslation;
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

	void initLights();

	//Render Calls
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	//Interaction Values
	bool InteractionBoundsCheck(Vector3 CameraPosition, int value);
	bool canPlacePlank = false;
	bool plankPlaced = false;
	Boundary PlankIxB;
	bool canInteractWithKyogre = false;
	bool interactedWithKyogre = false;
	Boundary KyogreIxB;
	void Particle::initBounds();
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

	void Particle::renderParticleEmitter(GLuint Texture, Vector3 Direction, Vector3 Velocity, int ParticleCount, int Spread);
};



#endif