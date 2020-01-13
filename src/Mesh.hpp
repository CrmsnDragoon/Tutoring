#pragma once
#include "DirectXMath.h"
#include "SceneNode.hpp"
#include "Material.hpp"
class Mesh {
	friend class GLModel;
private:
protected:
public:
	std::string name;
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT3> tangents;
	// ReSharper disable once IdentifierTypo
	std::vector<XMFLOAT3> binormals;
	std::vector<XMFLOAT3> texCoords[8];
	uint32_t numberActiveTexCoordChannels = 0;
	std::vector<XMFLOAT4> vertexColours[8];
	std::vector<uint32_t> indices;
	SceneNode* sceneNode = nullptr;
	uint32_t materialIndex;
	Material material;
};

// Short of an ID for a buffer on graphics card, everything required for animation and constructing a draw call
struct GameplayMesh {
	uint32_t stride;
	uint32_t indexCount;
	uint32_t offset;
	uint32_t firstVertex;
	SceneNode* sceneNode = nullptr;
};
