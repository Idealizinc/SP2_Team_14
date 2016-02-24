#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"
#include "Boundary.h"
#include "SharedData.h"
#include <vector>

extern int S_Width, S_Height;

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
	float leftGateA;
	float leftGateB;
	float rightGateA;
	float rightGateB;
	float frontGateA;
	float frontGateB;
	float backGateA;
	float backGateB;
	bool checkLeftGate;
	bool openLeftGate;
	bool checkRightGate;
	bool openRightGate;
	bool checkFrontGate;
	bool openFrontGate;
	bool checkBackGate;
	bool openBackGate;
	
private:
	Vector3 view;

    void rotateCamera(double dt);
    float CameraXrotation;
    float CameraYrotation, maxCameraXrotation, minCameraXrotation;
	void initBoundVec();
	float rotate_Sensitivity = S_Width / S_Height;
	float walkingSpeed = 10.f;
	float gravity;

    void cameraMovement(double dt);
    int boxX, boxZ;

    bool boundsCheck2D(const int& x, const int& z, float& posX, float& posZ);
    bool outOfBounds;
    int cameraMass;

	void cameraMovement2(double dt);

	float walkVal;

	std::vector<Boundary> BoundVec;
	
	float velocityY = 0;

	int jumpForce;
	int groundlevel;
	int jumpImpulse;
	int jumpSpeed;

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
	//Interactive
	Boundary TeleporterF1NW;
	Boundary TeleporterF1NE;
	Boundary TeleporterF1SW;
	Boundary TeleporterF1SE;

	Boundary leftGate;
	Boundary rightGate;
	Boundary frontGate;
	Boundary backGate;
};

#endif