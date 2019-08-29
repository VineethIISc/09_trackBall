#include "PLYreader.h"



PLYreader::PLYreader(const std::string& filename): plyNumVertCoord(0), fileFormat(0), numberOfTriangles(0), numberOfvertices(0)
{
	//read the file as stream
	std::ifstream inputFile(filename, std::ios::binary);
	if (!inputFile)
	{
		std::cout << "Failed to load the file: " << filename << std::endl;
		return;
	}

	std::string data;
	inputFile >> data;

	//check whether the input file is ply or not
	if (data == PLY_MAGIC_NUMBER)
	{
		//check for the format of input file 
		readHeader(inputFile);

		//read the file using respective methods
		if (fileFormat == PLY_ASCII)
			readASCIIply(inputFile);
		else
			readBINARYply(inputFile);
	}
	else
		std::cout << "File is not ply: " << filename << std::endl;
	

	//close the file
	inputFile.close();
}


int PLYreader::getNumberOfFaces()
{
	return numberOfTriangles;
}

void PLYreader::printNumberOfFaces()
{
	std::cout << "number of Faces: " << numberOfTriangles << std::endl;
}

int PLYreader::getNumberOfVertices()
{
	return numberOfvertices;
}

void PLYreader::printNumberOfVertices()
{
	std::cout << "number of Vertices: " << numberOfvertices << std::endl;
}

int PLYreader::getNumberOfIndices()
{
	return NUM_INDICES_TRI * getNumberOfFaces();
}

int PLYreader::getNumberOfVerticesbb()
{
	return BBvertices.size();
}

int PLYreader::getNumberOfIndicesbb()
{
	return NUM_INDICES_LINE * BBindices.size();
}

std::vector<Vector3f>& PLYreader::getVertices()
{
	return vertices;
}

std::vector<Vector3ui>& PLYreader::getIndices()
{
	return indices;
}

Vector3f PLYreader::getBBDimension()
{
	return BB.dimBox;
}

float PLYreader::getMaxDimensionBB()
{
	//compare among three values (someone can use max function)
	float max = BB.dimBox.x;

	if (max < BB.dimBox.y)
		max = BB.dimBox.y;
	
	if (max < BB.dimBox.z)
		max = BB.dimBox.z;

	return max;
}

Vector3f PLYreader::getBBCentroid()
{
	return BB.centroid;
}

std::vector<Vector3f>& PLYreader::getBBVertices()
{
	return BBvertices;
}

std::vector<Vector2ui>& PLYreader::getBBIndices()
{
	return BBindices;
}

void PLYreader::readHeader(std::ifstream& input)
{
	std::string data;
	
	//read till end of header
	do
	{
		input >> data;
		if (data == PLY_FORMAT)
		{
			input >> data;
			if (data == PLY_ASCII_FORMAT)
			{
				fileFormat = PLY_ASCII;
				continue;
			}
			else
			{
				fileFormat = PLY_BINARY;
				continue;
			}
		}
		if (data == PLY_VERTEX)
		{
			input >> numberOfvertices;
			input >> data;

			//next string "property"
			while (data == PLY_PROPERTY)
			{
				input >> data;
				if (data == PLY_FLOAT)
				{
					plyNumVertCoord++;
					std::string x;
					input >> x;
					input >> data;
				}
			}

		}
		else if (data == PLY_FACE)
		{
			input >> numberOfTriangles;
		}
	} while (data != PLY_END_HEADER);

}

void PLYreader::readASCIIply(std::ifstream& input)
{
	//read all vertices
	for (unsigned int i = 0; i < numberOfvertices; i++)
	{
		Vector3f v;
		input >> v.x >> v.y >> v.z;
		vertices.push_back(v);

		//compute min and max vertex BB
		computeMinMaxBB(v);
	}

	//read all faces
	for (unsigned int i = 0; i < numberOfTriangles; i++)
	{
		//read the first number in each face
		unsigned int start;
		input >> start;

		//check if it is 3 for triangle
		Vector3ui ind;
		if (start == NUM_INDICES_TRI)
			input >> ind.a >> ind.b >> ind.c;
		else
			std::cout << "Face is not triangle" << std::endl;

		indices.push_back(ind);
	}

	std::cout << "Done reading the PLY" << std::endl;


	//At the end compute the geometry of Bounding Box
	computeGeometryBB();
}

void PLYreader::readBINARYply(std::ifstream& input)
{
	
	//set the file get pointer to beginning
	input.seekg(0, std::ios::beg);

	//skip the header
	std::string line;
	do 
	{
		std::getline(input, line);
	}while (line != PLY_END_HEADER);


	//read all vertices
	for (unsigned int i = 0; i < numberOfvertices; i++)
	{
		//read a vertex: contain 3 coordinates
		Vector3f v;
		input.read((char*)& v, PLY_VERTEX_SIZE_TRI);
		vertices.push_back(v);

		//compute min and max vertex BB
		computeMinMaxBB(v);
	}

	//read all indices
	for (unsigned int i = 0; i < numberOfTriangles; i++)
	{
		//read the start value
		unsigned char start;
		input.read((char*)&start, PLY_FACE_START_NUM_SIZE);

		//read the faces: in term of indices
		if (start == NUM_INDICES_TRI)
		{
			Vector3ui ind;
			input.read((char*)& ind, PLY_INDICES_SIZE_TRI);
			indices.push_back(ind);
		}
		else
			std::cout << "Face is not triangle" << std::endl;
		
	}

	//At the end compute the geometry of Bounding Box
	computeGeometryBB();
	
}

void PLYreader::computeMinMaxBB(Vector3f& vertex)
{
	static bool onlyOnce = true;
	if (onlyOnce)
	{
		BB.minVert = vertex;
		BB.maxVert = vertex;
		onlyOnce = false;
	}
	else
	{
		//update the minimum value
		if (BB.minVert.x > vertex.x)
			BB.minVert.x = vertex.x;

		if (BB.minVert.y > vertex.y)
			BB.minVert.y = vertex.y;

		if (BB.minVert.z > vertex.z)
			BB.minVert.z = vertex.z;

		//update the maximum value
		if (BB.maxVert.x < vertex.x)
			BB.maxVert.x = vertex.x;

		if (BB.maxVert.y < vertex.y)
			BB.maxVert.y = vertex.y;

		if (BB.maxVert.z < vertex.z)
			BB.maxVert.z = vertex.z;
	}
}

void PLYreader::computeGeometryBB()
{
	//compute the dimension
	computeDimBB();

	//create the vertices
	createVertexBB();

	//create wireframe indices
	createWireFrameIndicesBB();
	
}

void PLYreader::computeDimBB()
{
	BB.centroid = (BB.minVert + BB.maxVert) * 0.5f;
	BB.dimBox = BB.maxVert - BB.minVert;
}

void PLYreader::createVertexBB()
{
	/*Bounding Box
		Defined with Vmin and Vmax (minimum vertex and maximum vertex)
		We have to create 8 vertices for the bounding box

			7------------6
		   ''			''
		  '	'		   ' '
		 '	'		  '  '
		3------------2   '
		'	'		 '   '
		'	4------------5		 
		'   '		 '  '
		'  '		 ' '
		' '			 ''
		0------------1
	*/

	//create vertices one faces
	BBvertices.push_back(BB.minVert);														//min vertex					-->0
	BBvertices.push_back(BB.minVert + Vector3f(BB.dimBox.x, 0.0f, 0.0f));					//change x- component			-->1
	BBvertices.push_back(BB.minVert + Vector3f(BB.dimBox.x, BB.dimBox.y, 0.0f));			//change x and y- component		-->2
	BBvertices.push_back(BB.minVert + Vector3f(0.0f, BB.dimBox.y, 0.0f));					//change y- component			-->3
																														
	//create vertices of other faces : change the z-component 															
	BBvertices.push_back(BB.minVert + Vector3f(0.0, 0.0f, BB.dimBox.z));					//change z- component			-->4
	BBvertices.push_back(BB.minVert + Vector3f(BB.dimBox.x, 0.0f, BB.dimBox.z));			//change x and z- component		-->5
	BBvertices.push_back(BB.minVert + Vector3f(BB.dimBox.x, BB.dimBox.y, BB.dimBox.z));		//change x, y and z- component	-->6
	BBvertices.push_back(BB.minVert + Vector3f(0.0f, BB.dimBox.y, BB.dimBox.z));			//change y and z- component		-->7
}																														

void PLYreader::createWireFrameIndicesBB()
{
	/*
		1)wireframe means drawing the bounding box with lines
		2)joining the vertices with lines
		3)box contain 12 edges
	*/
	BBindices.push_back(Vector2ui(0, 1));		//0-->1
	BBindices.push_back(Vector2ui(1, 2));		//1-->2
	BBindices.push_back(Vector2ui(2, 3));		//2-->3
	BBindices.push_back(Vector2ui(3, 0));		//3-->0

	BBindices.push_back(Vector2ui(4, 5));		//4-->5
	BBindices.push_back(Vector2ui(5, 6));		//5-->6
	BBindices.push_back(Vector2ui(6, 7));		//6-->7
	BBindices.push_back(Vector2ui(7, 4));		//7-->4

	BBindices.push_back(Vector2ui(0, 4));		//0-->4
	BBindices.push_back(Vector2ui(1, 5));		//1-->5
	BBindices.push_back(Vector2ui(2, 6));		//2-->6
	BBindices.push_back(Vector2ui(3, 7));		//3-->7
}


