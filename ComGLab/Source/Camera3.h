/****************************************************************************/
/*!
\file Camera3.h
\author Ryan Lim Rui An, Teh Kai Hong Bryan
\par email: 150577@.mymail.nyp.edu.sg, 150587A@mymail.nyp.edu.sg
\brief
Class to define camera
*/
/****************************************************************************/

#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"
#include "Boundary.h"
#include <vector>

extern int S_Width, S_Height;

/****************************************************************************/
/*!
Class Camera3:
\brief Defines the methods for camera control
*/
/****************************************************************************/

class Camera3 : public Camera
{
public:
    Vector3 defaultPosition;
    Vector3 defaultTarget;
    Vector3 defaultUp;

    Camera3();
    ~Camera3();

    virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up, int mass = 1);
    virtual void Update(double dt);
    virtual void Reset();

	Vector3 getCameraPosition();
	Vector3 getLookVector();

    bool isOutofBounds();
	bool teleCheck;
	bool shipCheck;
	bool coreCheck;
	bool spaceModeOn = false;
	float CameraXrotation, CameraYrotation, maxCameraXrotation, minCameraXrotation, maxCameraYrotation, minCameraYrotation;
	float baseWalkSpeed = 4;
	bool CanMoveCamera = true;

private:
	Vector3 view;

    void rotateCamera(double dt);
	void initBoundVec();
	float rotate_Sensitivity = S_Width / S_Height * 2;
	float walkingSpeed = 10.f;
	float gravity;

    void cameraMovement(double dt);
    int boxX, boxZ;

    bool boundsCheck2D(const int& x, const int& z, float& posX, float& posZ);

	void cameraMovement2(double dt);

	float walkVal;

	std::vector<Boundary> BoundVec;
	
	float velocityY = 0;

	bool isGravityOn;
	bool isMaxJump;
	bool isJump;
	float maxJump;
	float maxJump2;
	int level;
	void jump(double dt, int level);

	//Bounds
	//Refer to blueprints for bound location
	
	Boundary northwall1;
	Boundary southwall1;
	Boundary eastwall1;
	Boundary westwall1;
	Boundary northwall2;
	Boundary southwall2;
	Boundary eastwall2;
	Boundary westwall2;
	Boundary corebase;
	
	Boundary WorldBot;
	Boundary WorldTop;
	Boundary WorldLeft;
	Boundary WorldRight;
	Boundary WorldFront;
	Boundary WorldBack;

	Boundary Floor2Bot;
	Boundary Floor2Top;
	Boundary Floor2WestLeft;
	Boundary Floor2WestRight;
	Boundary Floor2EastLeft;
	Boundary Floor2EastRight;
	Boundary Floor2FrontLeft;
	Boundary Floor2FrontRight;
	Boundary Floor2BackLeft;
	Boundary Floor2BackRight;

	Boundary Base;
	//Interactive
	Boundary TeleporterF1NW;
	Boundary TeleporterF1NE;
	Boundary TeleporterF1SW;
	Boundary TeleporterF1SE;

	Boundary ShipCheck;
	Boundary CoreCheck;
};

#endif