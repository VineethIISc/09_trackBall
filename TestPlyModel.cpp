#include "Shader.h"
#include "PLYreader.h"
#include "TestPlyModel.h"

TestPlyModel::TestPlyModel(): ar(1.0f)
{
	plyreader = new PLYreader(MODEL_PATH"table.ply");

	//draw all vertices
	numVertices = plyreader->getNumberOfVertices();
	numIndices = plyreader->getNumberOfIndices();

	//get vertices and indices of bounding box
	bbNumVertices = plyreader->getNumberOfVerticesbb();
	bbNumIndices = plyreader->getNumberOfIndicesbb();

	//get the bounding box centroid and maximum dimension
	bbCentroid = plyreader->getBBCentroid();
	bbMaxDim = plyreader->getMaxDimensionBB();

	//create vertices and indices
	initData();
}


TestPlyModel::~TestPlyModel()
{
	if (VAO)
		glDeleteVertexArrays(1, &VAO);

	if (VBO)
		glDeleteBuffers(1, &VBO);

	if (plyreader)
		delete plyreader;

	if (objectShader)
		delete objectShader;
}

GLuint TestPlyModel::getVBO()
{
	return VBO;
}

GLuint TestPlyModel::getVAO()
{
	return VAO;
}

void TestPlyModel::loadData()
{
	//create vertex array objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//bind VAO, VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);	//element array buffer for indices

	//allocate memory in GPU
	GLsizeiptr VertexSize = sizeof(Vector3f) * numVertices;
	GLsizeiptr IndexSize = sizeof(unsigned int) * numIndices;
	GLsizeiptr TotalSize = VertexSize + IndexSize;
	glBufferData(GL_ARRAY_BUFFER, TotalSize, NULL, GL_STATIC_DRAW);

	//pass vertex position and color data
	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, VertexSize, vertices[0]);
	offset += VertexSize;
	//send index in the same buffer
	indexBufferOffset = offset;
	glBufferSubData(GL_ARRAY_BUFFER, offset, IndexSize, indices[0]);
	offset += IndexSize;

	//enable vertex attrib pointer and information about data
	glEnableVertexAttribArray(0);		//position


	//assign attribute pointer
	GLuint currBufferOffset = 0;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(currBufferOffset));
	currBufferOffset += VertexSize;


	//unbind VAO, VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);


	//load Data of bounding box
	loadBBdata();

	//create shader 
	objectShader = new Shader(SHADER_PATH"vsTestPlyModel.txt", SHADER_PATH"fsTestPlyModel.txt");
}

void TestPlyModel::render(Matrix4f& trackballOrientation)
{
	//bind the shader program
	objectShader->Bind();

	//translate to origin
	trans.InitTranslationTransform(-bbCentroid.x, -bbCentroid.y, -bbCentroid.z);
	
	//scale to NDC
	float scaleFactor = fabsf(1.0f / bbMaxDim);
	scal.InitScaleTransform(scaleFactor, scaleFactor, scaleFactor);

	//correcting the aspect ratio
	//when screen size is changed find the new aspect ratio
	arScale.InitScaleTransform(1/ar, 1.0f, 1.0f);

	////rotation matrix
	//static float angle = 0.0f;
	//angle += 0.5f;	//in degree
	////rot.InitRotateTransform(angle, 0.0f, 0.0f);
	//rot.InitRotateTransform(0.0f, angle, 0.0f);
	////rot.InitRotateTransform(0.0, 0.0f, angle);
	
	//final matrix
	finalMatrix = trackballOrientation * arScale * scal * trans;

	//send matrix to shader from application
	objectShader->setUniformVariable("modelToWorld", finalMatrix);

	//draw the model
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(indexBufferOffset));
	glBindVertexArray(0);

	//draw the bounding box
	glBindVertexArray(bbVAO);
	glDrawElements(GL_LINES, bbNumIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(bbIndexBufferOffset));
	glBindVertexArray(0);

	//unbind the shader
	objectShader->Unbind();
}

void TestPlyModel::setAspectRatio(float ratio)
{
	//aspect ratio change as the screen size change
	ar = ratio;
}

void TestPlyModel::initData()
{
	//store the vertices in local variable
	vertices = plyreader->getVertices();

	//store the indices in local variable
	indices = plyreader->getIndices();

	/*-----for bounding Box--------*/
	//store the vertices in local variable
	bbVertices = plyreader->getBBVertices();

	//store the indices in local variable
	bbIndices = plyreader->getBBIndices();
}

void TestPlyModel::loadBBdata()
{
	//create vertex array objects
	glGenVertexArrays(1, &bbVAO);
	glGenBuffers(1, &bbVBO);

	//bind VAO, VBO
	glBindVertexArray(bbVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bbVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bbVBO);	//element array buffer for indices

	//allocate memory in GPU
	GLsizeiptr bbVertexSize = sizeof(Vector3f) * bbNumVertices;
	GLsizeiptr bbIndexSize = sizeof(unsigned int) * bbNumIndices;
	GLsizeiptr TotalSize = bbVertexSize + bbIndexSize;
	glBufferData(GL_ARRAY_BUFFER, TotalSize, NULL, GL_STATIC_DRAW);

	//pass vertex position and color data
	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, bbVertexSize, bbVertices[0]);
	offset += bbVertexSize;
	//send index in the same buffer
	bbIndexBufferOffset = offset;
	glBufferSubData(GL_ARRAY_BUFFER, offset, bbIndexSize, bbIndices[0]);
	offset += bbIndexSize;

	//enable vertex attrib pointer and information about data
	glEnableVertexAttribArray(0);		//position


	//assign attribute pointer
	GLuint currBufferOffset = 0;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(currBufferOffset));
	currBufferOffset += bbVertexSize;


	//unbind VAO, VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);

}
