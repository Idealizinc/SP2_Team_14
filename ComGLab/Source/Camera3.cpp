#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"

#include "MyMath.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up, int mass)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();

	CameraXrotation = 0;
	CameraYrotation = 0;
	maxCameraXrotation = 89;
	minCameraXrotation = -89;

	rotate_Sensitivity = 2;
	cameraMass = 1;
	outOfBounds = false;
	initBoundVec();
}

void Camera3::Update(double dt)
{
	BoatPlank.active = plankBoundActive;
	HomeDoor.active = doorBoundActive;
	cameraMovement2(dt);
	outOfBounds = false;
	rotateCamera(dt);
}

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

void Camera3::rotateCamera(double dt)
{
	//Lock the cursor's position to the center of the screen.
	int moveX = S_Width / 2;
	int moveY = S_Height / 2;
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	SetCursorPos(moveX, moveY);
	CameraYrotation -= ((mousePosition.x - moveX) * (float)(dt)* rotate_Sensitivity);
	CameraXrotation += ((mousePosition.y - moveY) * (float)(dt)* rotate_Sensitivity);
	CameraXrotation = Math::Clamp(CameraXrotation, minCameraXrotation, maxCameraXrotation);

	target = Vector3(sin(Math::DegreeToRadian(CameraYrotation)) * cos(Math::DegreeToRadian(CameraXrotation)) + position.x,
		-sin(Math::DegreeToRadian(CameraXrotation)) + position.y,
		cos(Math::DegreeToRadian(CameraYrotation)) * cos(Math::DegreeToRadian(CameraXrotation)) + position.z);
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(defaultUp);
	up = right.Cross(view);
}

bool Camera3::boundsCheck2D(const int& xBound, const int& zBound, float& currXPos, float& currZPos)
{
	float pushBackValue = 0.001f;
	float offset = 1 / walkingSpeed;
	float xCopy, zCopy;
	xCopy = currXPos;
	zCopy = currZPos;
	//bound checks
	if (xBound < xCopy + offset)
	{
		currXPos -= pushBackValue;
		return false;
	}
	else if (zBound < zCopy + offset)
	{
		currZPos -= pushBackValue;
		return false;
	}
	else if (-xBound > xCopy - offset)
	{
		currXPos += pushBackValue;
		return false;
	}
	else if (-zBound > zCopy - offset)
	{
		currZPos += pushBackValue;
		return false;
	}
	//still within bounds
	else //if (currXPos <= xBound && currZPos <= zBound && currXPos >= -xBound && currZPos >= -zBound)
	{
		//outOfBounds = false;
		return true;
	}
}

void Camera3::cameraMovement(double dt)
{
	float offsetX, offsetZ;
	if (Application::IsKeyPressed('W') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('A') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('S') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('D') && boundsCheck2D(boxX, boxZ, position.x, position.z))
	{
		position.x += (float)(sin(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
		position.z += (float)(cos(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
	}
}

Vector3 Camera3::getCameraPosition()
{
	return position;
}

bool Camera3::isOutofBounds()
{
	return outOfBounds;
}

void Camera3::cameraMovement2(double dt)
{

	// Movement system, 2.0 where bounds checking is done within the move function.
	// Movement increments
	float walkingX = 0;
	float walkingZ = 0;
	if (Application::IsKeyPressed(VK_LSHIFT) || Application::IsKeyPressed(VK_RSHIFT))
	{
		walkingSpeed = 3;
	}
	else walkingSpeed = 5;

	if (Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 90)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 180)) / walkingSpeed);
	}
	if (Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
	{
		walkingX += (float)(sin(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
		walkingZ += (float)(cos(Math::DegreeToRadian(CameraYrotation + 270)) / walkingSpeed);
	}
	if (boundsCheckXY(walkingX + position.x, walkingZ + position.z))
	{
		position.x += walkingX;
		position.z += walkingZ;
	}
}
bool Camera3::boundsCheckXaxis(const int& x, const float& posX) {
	if (posX < x && posX > -x)
	{
		return true;
	}
	else return false;
}

bool Camera3::boundsCheckZaxis(const int& z, const float& posZ) {
	if (posZ < z && posZ > -z)
	{
		return true;
	}
	else return false;
}

void Camera3::initBoundVec()
{
	/*Note
	min =  trans - scale/2 ;
	max =  trans + scale/2;
	*/
	BoatBack.set(-10, 15, 18, 19);
	BoatLeft.set(-8, -6, 3, 27);
	BoatRight.set(12, 15, 3, 27);
	BoatFrontL.set(-32,-2,-19.5,5.5);
	BoatFrontR.set(1.4, 32.6, -19.5, 5.5);
	BoatPlank.set(-3, 3, -5.5, 5.5);
	Bridge.set(-3, 3, -15, 5);
	BaseL1.set(-59, -29, -39.5, 14.5);
	BaseL2.set(-79, -49, -157.5, -37.5);
	BaseR1.set(29, 59, -39.5, 14.5);
	BaseR2.set(49, 79, -66.2, -36.2);
	BaseR3.set(49, 79, -139.9, -69.9);
	BaseR4.set(69, 79, -71.2, -51.2);
	GrassR.set(9.5, 54.5, -143, -118);
	GrassL.set(-54.5, -9.5, -143, -118);
	GrassC.set(-45, 45, -163, -135);
	HomeB.set(-20, 20, -136.5, -136);
	HomeL.set(-10, -7.35, -136, -120);
	HomeR.set(7.4, 10, -136, -120);
	HomeFL.set(-7.75, 1.75, -120.6, -119.2);
	HomeFR.set(4.45, 8.45, -120.6, -119.2);
	HomeTable.set(-8, -2, -136.125, -131.875);
	HomeDoor.set(1, 5, -120.6, -119.8);
	HousesL.set(-54.5, -9.5, -116, -104);
	HousesR.set(9.5, 54.5, -116, -104);
	HousesL2.set(-54.5, -9.5, -86, -74);
	HousesR2.set(20.5, 54.5, -86, -74);
	HousesL3.set(-50, -38, -57, -41);
	HousesR3.set(38, 50, -57, -41);
	HousesL4.set(-30, -15, -30.5, -19.5);
	HousesR4.set(15, 30, -30.5, -19.5);
	Tower1.set(-3, 3, -63, -57);
}


bool Camera3::boundsCheckXY(const float& posX, const float& posZ)
{
	// Check if player will get into a box they aren't allowed to be within
	// Return true if not in any of such boxes
	/*if (BoatBack.active == true &&
		posX < BoatBack.xMax && posX > BoatBack.xMin &&
		posZ < BoatBack.zMax && posZ > BoatBack.zMin)
	{
		return false;
	}
	if (BoatLeft.active == true &&
		posX < BoatLeft.xMax && posX > BoatLeft.xMin &&
		posZ < BoatLeft.zMax && posZ > BoatLeft.zMin)
	{
		return false;
	}
	if (BoatRight.active == true &&
		posX < BoatRight.xMax && posX > BoatRight.xMin &&
		posZ < BoatRight.zMax && posZ > BoatRight.zMin)
	{
		return false;
	}
	if (BoatFrontL.active == true &&
		posX < BoatFrontL.xMax && posX > BoatFrontL.xMin &&
		posZ < BoatFrontL.zMax && posZ > BoatFrontL.zMin)
	{
		return false;
	}
	if (BoatFrontR.active == true &&
		posX < BoatFrontR.xMax && posX > BoatFrontR.xMin &&
		posZ < BoatFrontR.zMax && posZ > BoatFrontR.zMin)
	{
		return false;
	}
	if (BoatPlank.active == false &&
		posX < BoatPlank.xMax && posX > BoatPlank.xMin &&
		posZ < BoatPlank.zMax && posZ > BoatPlank.zMin)
	{
		return false;
	}
	if (BaseR1.active == true &&
		posX < BaseR1.xMax && posX > BaseR1.xMin &&
		posZ < BaseR1.zMax && posZ > BaseR1.zMin)
	{
		return false;
	}
	if (BaseL1.active == true &&
		posX < BaseL1.xMax && posX > BaseL1.xMin &&
		posZ < BaseL1.zMax && posZ > BaseL1 .zMin)
	{
		return false;
	}
	if (BaseL2.active == true &&
		posX < BaseL2.xMax && posX > BaseL2.xMin &&
		posZ < BaseL2.zMax && posZ > BaseL2.zMin)
	{
		return false;
	}
	if (BaseR2.active == true &&
		posX < BaseR2.xMax && posX > BaseR2.xMin &&
		posZ < BaseR2.zMax && posZ > BaseR2.zMin)
	{
		return false;
	}
	if (BaseR3.active == true &&
		posX < BaseR3.xMax && posX > BaseR3.xMin &&
		posZ < BaseR3.zMax && posZ > BaseR3.zMin)
	{
		return false;
	}
	if (BaseR4.active == true &&
		posX < BaseR4.xMax && posX > BaseR4.xMin &&
		posZ < BaseR4.zMax && posZ > BaseR4.zMin)
	{
		return false;
	}
	if (GrassR.active == true &&
		posX < GrassR.xMax && posX > GrassR.xMin &&
		posZ < GrassR.zMax && posZ > GrassR.zMin)
	{
		return false;
	}
	if (GrassL.active == true &&
		posX < GrassL.xMax && posX > GrassL.xMin &&
		posZ < GrassL.zMax && posZ > GrassL.zMin)
	{
		return false;
	}
	if (GrassC.active == true &&
		posX < GrassC.xMax && posX > GrassC.xMin &&
		posZ < GrassC.zMax && posZ > GrassC.zMin)
	{
		return false;
	}
	if (HomeB.active == true &&
		posX < HomeB.xMax && posX > HomeB.xMin &&
		posZ < HomeB.zMax && posZ > HomeB.zMin)
	{
		return false;
	}
	if (HomeL.active == true &&
		posX < HomeL.xMax && posX > HomeL.xMin &&
		posZ < HomeL.zMax && posZ > HomeL.zMin)
	{
		return false;
	}
	if (HomeR.active == true &&
		posX < HomeR.xMax && posX > HomeR.xMin &&
		posZ < HomeR.zMax && posZ > HomeR.zMin)
	{
		return false;
	}
	if (HomeTable.active == true &&
		posX < HomeTable.xMax && posX > HomeTable.xMin &&
		posZ < HomeTable.zMax && posZ > HomeTable.zMin)
	{
		return false;
	}
	if (HomeFL.active == true &&
		posX < HomeFL.xMax && posX > HomeFL.xMin &&
		posZ < HomeFL.zMax && posZ > HomeFL.zMin)
	{
		return false;
	}
	if (HomeFR.active == true &&
		posX < HomeFR.xMax && posX > HomeFR.xMin &&
		posZ < HomeFR.zMax && posZ > HomeFR.zMin)
	{
		return false;
	}
	if (HomeDoor.active == false &&
		posX < HomeDoor.xMax && posX > HomeDoor.xMin &&
		posZ < HomeDoor.zMax && posZ > HomeDoor.zMin)
	{
		return false;
	}
	if (HousesL.active == true &&
		posX < HousesL.xMax && posX > HousesL.xMin &&
		posZ < HousesL.zMax && posZ > HousesL.zMin)
	{
		return false;
	}
	if (HousesL2.active == true &&
		posX < HousesL2.xMax && posX > HousesL2.xMin &&
		posZ < HousesL2.zMax && posZ > HousesL2.zMin)
	{
		return false;
	}
	if (HousesR.active == true &&
		posX < HousesR.xMax && posX > HousesR.xMin &&
		posZ < HousesR.zMax && posZ > HousesR.zMin)
	{
		return false;
	}
	if (HousesR2.active == true &&
		posX < HousesR2.xMax && posX > HousesR2.xMin &&
		posZ < HousesR2.zMax && posZ > HousesR2.zMin)
	{
		return false;
	}
	if (HousesL3.active == true &&
		posX < HousesL3.xMax && posX > HousesL3.xMin &&
		posZ < HousesL3.zMax && posZ > HousesL3.zMin)
	{
		return false;
	}
	if (HousesR3.active == true &&
		posX < HousesR3.xMax && posX > HousesR3.xMin &&
		posZ < HousesR3.zMax && posZ > HousesR3.zMin)
	{
		return false;
	}
	if (HousesL4.active == true &&
		posX < HousesL4.xMax && posX > HousesL4.xMin &&
		posZ < HousesL4.zMax && posZ > HousesL4.zMin)
	{
		return false;
	}
	if (HousesR4.active == true &&
		posX < HousesR4.xMax && posX > HousesR4.xMin &&
		posZ < HousesR4.zMax && posZ > HousesR4.zMin)
	{
		return false;
	}
	if (Tower1.active == true &&
		posX < Tower1.xMax && posX > Tower1.xMin &&
		posZ < Tower1.zMax && posZ > Tower1.zMin)
	{
		return false;
	}
	else*/
	return true;
}

