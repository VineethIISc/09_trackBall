#include "Shader.h"
#include "PLYreader.h"
#include "Frame.h"

Frame::Frame(): ar(1.0f)
{
	//use the ply reader to render the mesh of arrow
	plyreader = new PLYreader(MODEL_PATH"arrow.ply");

	//draw all vertices
	numVertices = plyreader->getNumberOfVertices();
	numIndices = plyreader->getNumberOfIndices();

	//get vertices and indices of bounding box
	bbNumVertices = plyreader->getNumberOfVerticesbb();
	bbNumIndices = plyreader->getNumberOfIndicesbb();

	//get the bounding box centroid and maximum dimension
	bbCentroid = plyreader->getBBCentroid();
	bbMaxDim = plyreader->getMaxDimensionBB();
	bbDimension = plyreader->getBBDimension();

	//create vertices and indices
	initData();

	//generate the colors
	color.resize(NUM_ARROWS);
	color[0] = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);		//red color
	color[1] = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);		//green color
	color[2] = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);		//blue color

	/*Translate the arrow's centroid to origin*/
	transToOrigin.InitTranslationTransform(-bbCentroid.x, -bbCentroid.y, -bbCentroid.z);
	
	/*translate from origin in x-axis
	remember that arrow is scale down
	so translate :	(half of bbDimension.x)		*/
	float xDisplacement = bbDimension.x * HALF;
	transInX.InitTranslationTransform(xDisplacement, 0.0f, 0.0f);

	/*Scale the arrow to normalized value:
	Then stretch it along the x-axis by some factor*/
	float scaleFactor = fabsf(1.0f / bbMaxDim) / ARROW_SCLAE_DOWN_FACTOR;
	scaleToNDC.InitScaleTransform(scaleFactor * ARROW_SCALE_UP_FACTOR, scaleFactor, scaleFactor);

	/*transformation that align the arrow as x-axis:
		1) translate to origin
		2) scale down the mesh to normalize coord
		3) translate x-distance to have a look of arrow originating from origin
	*/
	finalArrowTrans = scaleToNDC * transInX * transToOrigin;

	/*Transformation required to create triad of frame*/
	rotation.resize(NUM_ARROWS);
	rotation[0].InitIdentity();							// arrow is aligned with the x-axis
	rotation[1].InitRotateTransform(0.0f, 0.0f, 90.0f);	//rotate 90 degree about z-axis for Y arrow	
	rotation[2].InitRotateTransform(0.0f, 90.0f, 0.0f); //rotate 90 degree about y-axis for YZ circle
}


Frame::~Frame()
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


void Frame::render(Matrix4f& trackballOrientation)
{
	//bind the shader program
	objectShader->Bind();

	//correcting the aspect ratio
	//when screen size is changed find the new aspect ratio
	arScale.InitScaleTransform(1.0f / ar, 1.0f, 1.0f);

	//final matrix to send to shader
	Matrix4f finalConcMatrix;

	//send Data to shader from application
	for (unsigned int i = 0; i < NUM_ARROWS; i++)
	{
		std::stringstream ss;
		std::string index;
		ss << i;
		index = ss.str();

		finalConcMatrix = trackballOrientation * rotation[i] *  arScale * finalArrowTrans;

		std::string name;
		name = "transMatrix[" + index + "]";
		objectShader->setUniformVariable(name, finalConcMatrix);
		name = "inColor[" + index + "]";
		objectShader->setUniformVariable(name, color[i]);
	}

	//draw the model
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(indexBufferOffset), NUM_ARROWS);
	glBindVertexArray(0);

	////draw the bounding box
	//glBindVertexArray(bbVAO);
	//glDrawElements(GL_LINES, bbNumIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(bbIndexBufferOffset));
	//glBindVertexArray(0);

	//unbind the shader
	objectShader->Unbind();
}

void Frame::setAspectRatio(float ratio)
{
	//aspect ratio change with scree size
	ar = ratio;
}

void Frame::initData()
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

void Frame::loadData()
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
	objectShader = new Shader(SHADER_PATH"vsInstancedShader.txt", SHADER_PATH"fsInstancedShader.txt");
}


void Frame::loadBBdata()
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

GLuint Frame::getVBO()
{
	return VBO;
}

GLuint Frame::getVAO()
{
	return VAO;
}
