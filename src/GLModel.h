#pragma once
#include <vector>
#include <string>
#include <assimp/scene.h>


class GLModel
{
public:
	GLModel(const std::string& modelPath = R"(G:\Projects\Tutoring\GLFW-Assimp-Example\assets\cube.fbx)");
	bool ImportStaticModel(const aiScene* scene);
	bool ImportMesh(const aiMesh* mesh);
	bool virtual LoadModel(const std::string& pathToModel);
	void virtual DrawGL_1_0();
	void virtual DrawGL_3_2();
	bool virtual IsLoaded();
protected:
	std::vector<float> positions;
	std::vector<uint32_t> indexes;
	std::vector<float> normals;
	std::vector<float> tangents;
	std::vector<float> bitangents;
	std::vector<float> texCoords;
	std::vector<float> colours;
};