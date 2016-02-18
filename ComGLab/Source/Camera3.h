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
	void initBoundVec();
private:
    void rotateCamera(double dt);
    float CameraXrotation;
    float CameraYrotation, maxCameraXrotation, minCameraXrotation;

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
	Boundary BoatBack;
	Boundary BoatLeft;
	Boundary BoatRight;
	Boundary BoatFrontL;
	Boundary BoatFrontR;
	Boundary Bridge;
	Boundary BaseR1;
	Boundary BaseL1;
	Boundary BaseR2;
	Boundary BaseL2;
	Boundary BaseR3;
	Boundary BaseR4;
	Boundary GrassR;
	Boundary GrassL;
	Boundary GrassC;
	Boundary HomeB;
	Boundary HomeL;
	Boundary HomeR;
	Boundary HomeFL;
	Boundary HomeFR;
	Boundary HomeTable;

	Boundary HousesL;
	Boundary HousesR;
	Boundary HousesL2;
	Boundary HousesR2;
	Boundary HousesL3;
	Boundary HousesR3;
	Boundary HousesL4;
	Boundary HousesR4;
	Boundary Tower1;

	//Interactive
	Boundary BoatPlank;
	Boundary HomeDoor;
};

#endif