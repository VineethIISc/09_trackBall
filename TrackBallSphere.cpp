#include "Shader.h"
#include "TrackBallSphere.h"

TrackBallSphere::TrackBallSphere():radius(RADIUS_SPHERE), numberOfPtsOnCircle(POINTS_ON_CIRCLE), VAO(0), 
VBO(0)
{
	//create the mesh that represent the trackball sphere
	createPtsOnCircle();

	//generate the colors
	color.resize(NUM_CIRCLES);
	color[0] = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);		//red color
	color[1] = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);		//green color
	color[2] = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);		//blue color

	//no transformation for XY circle 
	rotation.resize(NUM_CIRCLES);
	rotation[0].InitIdentity();
	rotation[1].InitRotateTransform(90.0f, 0.0f, 0.0f);	//rotate 90 degree about x-axis for XZ circle	
	rotation[2].InitRotateTransform(0.0f, 90.0f, 0.0f); //rotate 90 degree about y-axis for YZ circle
}


TrackBallSphere::~TrackBallSphere()
{
	//delete buffer if there are allotted
	if (VAO)
		glDeleteVertexArrays(1, &VAO);

	if (VBO)
		glDeleteBuffers(1, &VBO);

	if (objectShader)
		delete objectShader;

}

GLuint TrackBallSphere::getVBO()
{
	return VBO;
}

GLuint TrackBallSphere::getVAO()
{
	return VAO;
}

void TrackBallSphere::createPtsOnCircle()
{
	//store x and y coordinate 
	vertices.resize(numberOfPtsOnCircle);
	
	//compute the step angle to generate the points
	double stepSize = 2.0 * M_PI / (float)(numberOfPtsOnCircle);

	//generate the points on the circle
	for (unsigned int i = 0; i < numberOfPtsOnCircle; i++)
	{
		//compute x and y coordinate 
		float xCoord = radius * cos(stepSize * i);
		float yCoord = radius * sin(stepSize * i);

		//store XY circle (Z = 0.0)
		vertices[i] = Vector3f(xCoord, yCoord, 0.0f);
	}
}

void TrackBallSphere::render(Matrix4f& trackballOrientation)
{
	//bind the shader program
	objectShader->Bind();
	
	//correcting the aspect ratio
	//when screen size is changed find the new aspect ratio
	arScale.InitScaleTransform(1.0f / ar, 1.0f, 1.0f);

	//final matrix to send to shader
	Matrix4f finalConcMatrix;

	//send Data to shader from application
	for (unsigned int i = 0; i < NUM_CIRCLES; i++)
	{
		std::stringstream ss;
		std::string index;
		ss << i;
		index = ss.str();

		finalConcMatrix = trackballOrientation * rotation[i] * arScale;
		
		std::string name;
		name = "transMatrix[" + index + "]";
		objectShader->setUniformVariable(name, finalConcMatrix);
		name = "inColor[" + index + "]";
		objectShader->setUniformVariable(name, color[i]);
	}
	
	//draw using instancing
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_LINE_LOOP, 0, numberOfPtsOnCircle, NUM_CIRCLES);
	
	//unbind VAO
	glBindVertexArray(0);

	//unbind the shader
	objectShader->Unbind();
}

void TrackBallSphere::setAspectRatio(float ratio)
{
	//aspect ratio change with scree size
	ar = ratio;
}

void TrackBallSphere::loadData()
{
	//create vertex array objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//bind VAO, VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	//allocate memory in GPU
	GLsizeiptr VertexSize = sizeof(Vector3f) * numberOfPtsOnCircle;
	GLsizeiptr TotalSize = VertexSize;
	glBufferData(GL_ARRAY_BUFFER, TotalSize, NULL, GL_STATIC_DRAW);

	//pass vertex position and color data
	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, VertexSize, vertices[0]);
	offset += VertexSize;
	
	//enable vertex attrib pointer and information about data
	glEnableVertexAttribArray(0);		//position

	//assign attribute pointer
	GLuint currBufferOffset = 0;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(currBufferOffset));
	currBufferOffset += VertexSize;

	//unbind VAO, VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	
	//create shader 
	objectShader = new Shader(SHADER_PATH"vsInstancedShader.txt", SHADER_PATH"fsInstancedShader.txt");
}