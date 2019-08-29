#ifndef PLY_MODEL_H
#define PLY_MODEL_H

class Shader;
class PLYreader;

class TestPlyModel
{
public:
	TestPlyModel();
	~TestPlyModel();

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
	float bbMaxDim;

	void loadBBdata();

private:
	//send the matrix 
	Matrix4f rot, scal, trans, finalMatrix;
	//aspect ratio
	float ar;	
	Matrix4f arScale;
	
};
#endif

