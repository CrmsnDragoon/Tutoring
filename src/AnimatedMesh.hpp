#pragma once
#include "DirectXMath.h"
#include "Mesh.hpp"
#include "Bone.hpp"

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
};
