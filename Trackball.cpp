#include "Trackball.h"



Trackball::Trackball()
{
	OrientationMatrix.InitIdentity();
}


Trackball::~Trackball()
{
}

void Trackball::setPosition(Vector3f& position)
{
	//Each track ball is attached with the object it has to manipulate
	m_position = position;
}

void Trackball::setOrientation(Vector3f& orientation)
{
}

void Trackball::setMousePosition(Vector2f& lastPosition)
{
	//get the 3d position of point
	m_lastPosition = projectPointToUnitSphere(lastPosition);
}

void Trackball::computeTrackballAxisAngle(Vector2f& currentPosition)
{
	//get the 3d position of point
	m_currentPosition = projectPointToUnitSphere(currentPosition);

	//axis of rotation
	m_axisOfRot = m_lastPosition.Cross(m_currentPosition);

	/*angle is small sin(angle) is approx to angle:
		magnitude of the axisOfRotation is sin(angle) in this case
		because the vectors are unit vector (on unit sphere)
		scale down using some factor*/
	float Angle = m_axisOfRot.length();
	if (Angle > 1e-03)
	{
		// Angle of rotation
		m_angleOfRot = Angle * SCALE_DOWN_FACTOR;

		//compute unit axis of rotation
		m_axisOfRot.Normalize();	

		//calculate the rotation matrix using angle-axis matrix
		Matrix4f CurrRotationMatrix;
		CurrRotationMatrix.InitAxisRotateTransform(m_axisOfRot, m_angleOfRot);

		//update the orientation matrix
		OrientationMatrix = CurrRotationMatrix * OrientationMatrix;

		// Update the last position
		m_lastPosition = m_currentPosition;
	}

}

Matrix4f& Trackball::getTrackBallOrientation()
{
	return OrientationMatrix;
}


Vector3f Trackball::projectPointToUnitSphere(Vector2f& lastPosition)
{
	//compute x^2 + y^2
	float d = lastPosition.x * lastPosition.x + lastPosition.y * lastPosition.y;
	
	//z -component of point 
	float z;

	//check whether the mouse click is within the unit circle in XY plane
	//please check reference about the computation
	float treshold = RADIUS_OF_SPHERE * RADIUS_OF_SPHERE * HALf;
	if (d < treshold)
	{
		/*Project a point to Sphere*/
		//calculate the z-coordinate in unit sphere z = sqrt(radius^2 - (x^2 + y^2))
		z = sqrtf (RADIUS_OF_SPHERE * RADIUS_OF_SPHERE - d);

		//project the mouse point to unit sphere
		return Vector3f(lastPosition.x, lastPosition.y, z);
	}
	else 
		z = treshold / sqrtf(d);

	return Vector3f(lastPosition.x, lastPosition.y, z);
}
