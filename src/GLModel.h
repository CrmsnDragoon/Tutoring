#pragma once
#include <vector>
#include <string>
#include <assimp/scene.h>


class GLModel
{
public:
	GLModel(const std::string& modelPath = R"(G:\Projects\Tutoring\GLFW-Assimp-Example\assets\cube.fbx)");
	bool ImportStaticModel(const aiScene* scene);
	bool ImportMesh(const aiMesh* mesh, const aiScene* scene);
	bool virtual LoadModel(const std::string& pathToModel);
	void SetupGL_1_0();
	void virtual DrawGL_1_0();
	void SetupGL_3_2();
	void virtual DrawGL_3_2();
	bool virtual IsLoaded();
protected:
	unsigned vertexArrayBuffer;
	unsigned vertexBuffer;
	unsigned indexBuffer;
	std::vector<float> positions;
	std::vector<uint32_t> indexes;
	std::vector<float> normals;
	std::vector<float> tangents;
	std::vector<float> bitangents;
	std::vector<float> texCoords;
	std::vector<float> colours;
};