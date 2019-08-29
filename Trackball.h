/*reference; https://www.khronos.org/opengl/wiki/Object_Mouse_Trackball*/

#ifndef TRACK_BALL_H
#define TRACK_BALL_H

#include "CommonHeader.h"

#define RADIUS_OF_SPHERE 1.0
#define SCALE_DOWN_FACTOR 0.1
#define HALf 0.5

class Trackball
{
public:
	Trackball();
	~Trackball();

	//setters
	void setPosition(Vector3f& position);			//set the position of trackball
	void setOrientation(Vector3f& orientation);		//set the orientation of trackball
	void setMousePosition(Vector2f& lastPosition);	//set the last position of mouse

	//compute axis of Rotation and angle
	void computeTrackballAxisAngle(Vector2f& currentPosition);

	//getters
	Matrix4f& getTrackBallOrientation();

private:
	//Trackball position
	Vector3f m_position;

	//Trackball orientation
	Vector3f m_orientation;

	//trackball axis and angle
	Vector3f m_axisOfRot;
	double m_angleOfRot;
	Matrix4f OrientationMatrix;

	//point on in unit sphere
	Vector3f m_lastPosition;
	Vector3f m_currentPosition;


	//project the mouse point into surface of unit sphere
	Vector3f projectPointToUnitSphere(Vector2f& lastPosition);

	
};
#endif

