/* Mesh that represent the trackball
	Implementation details:
	   1) Vertices for one circle (XY plane) is generated
	   2) To create other three used rotation instance of XY circle
	   3) XY circle in red, YZ in green, XZ in blue
	   4) Pass an array matrices as uniform variable to shader
	   5) Pass an array of color as uniform variable to shader
	   6) Use gl_InstanceID to retrieve the matrices
  */

#ifndef TRACK_BALL_SPHERE
#define TRACK_BALL_SPHERE

#include "CommonHeader.h"

#define RADIUS_SPHERE 0.5
#define POINTS_ON_CIRCLE 100
#define NUM_CIRCLES 3

class Shader;

class TrackBallSphere
{
public:
	TrackBallSphere();
	~TrackBallSphere();

	//debug
	GLuint getVBO();
	GLuint getVAO();

	//rendering purpose
	void loadData();
	void render(Matrix4f& trackballOrientation);

	//setters
	void setAspectRatio(float ratio);

private:
	//buffer storage
	GLuint VBO, VAO;

	//store vertices and indices store xy circle 
	std::vector<Vector3f> vertices;
		
	//radius of sphere
	double radius;

	//Number of vertices that represent the circle
	unsigned int numberOfPtsOnCircle;

	//generate a xy circle (z = 0)
	void createPtsOnCircle();

	//create a shader for the current object from source file
	Shader * objectShader;

	//aspect ratio
	float ar;
	Matrix4f arScale;

	/*color for cicle:
	red : XY circle
	green : YZ circle
	blue : XZ circle
	*/
	std::vector<Vector4f>color;
	std::vector<Matrix4f>rotation;
};

#endif // !TRACK_BALL_SPHERE

