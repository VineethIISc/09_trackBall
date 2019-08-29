/*Frame fore each object is required to manipulate the object
Implementation:
	1) A mesh of arrow that is aligned with x-axis is drawn using catia and store in ply format
	2) Use ply reader to import the mesh and send data to GPU
	3) Create triad of arrow to construct the frame
		a) rotating the x-axis arrow about z-axis in counter clockwise
		b) rotating the x-axis arrow about y-axis in counter clockwise
*/

#ifndef FRAME_H
#define FRAME_H

#include "CommonHeader.h"

#define NUM_ARROWS 3
#define ARROW_SCALE_UP_FACTOR 3		//scale up along the x-axis
#define ARROW_SCLAE_DOWN_FACTOR 10	//uniform scale down
#define HALF 0.5

class Shader;
class PLYreader;

class Frame
{
public:
	Frame();
	~Frame();

	//debug
	GLuint getVBO();
	GLuint getVAO();

	//rendering purpose
	void loadData();
	void render(Matrix4f& trackballOrientation);

	//setters
	void setAspectRatio(float ratio);
	

private:
	GLuint VBO, VAO;
	//store vertices and indices
	std::vector<Vector3f> vertices;
	std::vector<Vector3ui> indices;
	int numVertices, numIndices;
	GLuint indexBufferOffset;

	//triangle vertices;
	void initData();

	//create a shader for the current object from source file
	Shader* objectShader;

	//read from stl file
	PLYreader* plyreader;

	/*-----for bounding Box--------*/
	//store vertices and indices
	GLuint bbVBO, bbVAO;
	std::vector<Vector3f> bbVertices;
	std::vector<Vector2ui> bbIndices;
	int bbNumVertices, bbNumIndices;
	GLuint bbIndexBufferOffset;
	Vector3f bbCentroid;
	Vector3f bbDimension;
	float bbMaxDim;

	void loadBBdata();

private:
	//send the matrix 
	Matrix4f scaleToNDC, transToOrigin, transInX, finalArrowTrans;	//matrix to show the arrow
	
	//aspect ratio
	float ar;
	Matrix4f arScale;

	/*color for arrow:
	red : X arrow
	green : Y arrow
	blue : Z arrow
	*/
	std::vector<Vector4f>color;
	std::vector<Matrix4f>rotation;

};

#endif // !TRACK_BALL_SPHERE

