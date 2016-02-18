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

    bool isOutofBounds();
	
private:
    void rotateCamera(double dt);
    float CameraXrotation;
    float CameraYrotation, maxCameraXrotation, minCameraXrotation;
	void initBoundVec();
	float rotate_Sensitivity = S_Width / S_Height * 2;
	float walkingSpeed = 5.f;

    void cameraMovement(double dt);
    int boxX, boxZ;

    bool boundsCheck2D(const int& x, const int& z, float& posX, float& posZ);
    bool outOfBounds;
    int cameraMass;

	void cameraMovement2(double dt);

	float walkVal;

	std::vector<Boundary> BoundVec;
	

	//Bounds
	//Refer to blueprints for bound location
<<<<<<< HEAD
	
	Boundary northwall1;
	Boundary southwall1;
	Boundary eastwall1;
	Boundary westwall1;
	Boundary northwall2;
	Boundary southwall2;
	Boundary eastwall2;
	Boundary westwall2;
	Boundary northgate1;
	Boundary southgate1;
	Boundary eastgate1;
	Boundary westgate1;
	Boundary northgate2;
	Boundary southgate2;
	Boundary eastgate2;
	Boundary westgate2;
	Boundary corebase;
	
	

	//Interactive
	
=======
	Boundary BoatBack;
	Boundary BoatFront;

	Boundary WorldBot;
	Boundary WorldTop;
	Boundary WorldLeft;
	Boundary WorldRight;
	Boundary WorldFront;
	Boundary WorldBack;

	Boundary Floor2Bot;
	Boundary Floor2Top;
	Boundary Floor2Left;
	Boundary Floor2Right;
	Boundary Floor2Front;
	Boundary Floor2Back;
	//Interactive

>>>>>>> e27f3d3092bf545e5d1f11e68482a300b5973bdf
};

#endif