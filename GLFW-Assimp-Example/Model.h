#pragma once
#include <vector>
#include <string>
#include <Assimp/scene.h>

struct Vec2 {
	float x;
	float y;
};
struct Vec3 {
	float x;
	float y;
	float z;
};
struct Vec4 {
	float x;
	float y;
	float z;
	float w;
};


class Model
{
public:
	Model(const std::string& modelPath = R"(G:\Projects\Tutoring\GLFW-Assimp-Example\assets\cube.fbx)");
	bool ImportStaticMesh(const aiScene* scene);
	bool ImportMesh(const aiMesh* mesh);
	bool LoadModel(const std::string& pathToModel);
	void DrawGL_1_0();
	bool IsLoaded();
private:
	std::vector<float> positions;
	std::vector<uint32_t> indexes;
	std::vector<float> texCoords;
	std::vector<float> colours;
};

