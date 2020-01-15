#pragma once
#include "DirectXMath.h"
#include "Mesh.hpp"
#include "Bone.hpp"
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <glad/glad.h>

class Bone;
struct BoneWeight_interstitial {
	std::vector<std::string> boneNames;
	std::vector<uint32_t> indexes;
	std::vector<float> weights;
};

struct BoneWeight {
	XMUINT4 boneIndexes;
	XMFLOAT4 boneWeights;
};


class AnimatedMesh : public Mesh
{
private:
protected:
public:
	std::vector<Bone*> bones{};
	std::vector<BoneWeight> boneWeights{};
	unsigned vertexArrayBuffer;
	unsigned vertexBuffer;
	unsigned indexBuffer;
	void setupGL();
	void Draw() {
        // draw mesh
        glBindVertexArray(vertexArrayBuffer);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
	}
};
