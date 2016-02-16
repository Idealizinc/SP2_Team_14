#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H
#include "Vertex.h"
#include "Mesh.h"
#include "LoadOBJ.h"


/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string &meshName, Color color);
	static Mesh* GenerateCube(const std::string &meshName, Color color);
	static Mesh* GenerateCircle(const std::string &meshName, Color color, unsigned numSlices = 36); //numSizes refers to resolution
	static Mesh* GenerateRing(const std::string &meshName, Color color, float outerRadius = 1.2, float innerRadius = 1.2, unsigned numSlices = 36); //Assuming outer Rad = 1
	static Mesh* GenerateSphere(const std::string &meshName, Color color, unsigned numStacks = 36, unsigned numSlices = 36);
	
	//Additional Meshes
	static Mesh* GenerateHemisphere(const std::string &meshName, Color color, unsigned numStacks = 36, unsigned numSlices = 36);
	static Mesh* GenerateQuarterSphere(const std::string &meshName, Color color, unsigned numStacks = 36, unsigned numSlices = 36);
	static Mesh* GenerateTorus(const std::string &meshName, Color color, float outerRadius = 1, float innerRadius = 1, unsigned numStacks = 36, unsigned numSlices = 36);
	static Mesh* GenerateHalfTorus(const std::string &meshName, Color color, float outerRadius = 1, float innerRadius = 1, float numStacks = 36, unsigned numSlices = 36);
	static Mesh* GenerateCylinder(const std::string &meshName, Color color, unsigned numStacks = 36);
	static Mesh* GenerateCone(const std::string &meshName, Color color, unsigned numStacks = 36);
	static Mesh* GenerateTrapezium(const std::string &meshName, Color color);
	static Mesh* GeneratePrism(const std::string &meshName, Color color);

	//Custom Meshes
	static Mesh* GenerateOBJ(const std::string &meshName, const std::string &file_path);
	//text gen
	static Mesh* GenerateText(const std::string &meshName, unsigned numRow, unsigned numCol);
	//Particle
	Mesh* GenerateParticle(const std::string &meshName, Color color);
};

#endif