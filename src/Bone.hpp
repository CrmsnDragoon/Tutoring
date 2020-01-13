#pragma once
#include <DirectXMath.h>
#include "SceneNode.hpp"

class Bone : public SceneNode {
public:
	Bone() = default;
	Bone(const std::string& name, uint32_t index, const XMFLOAT4X4& offsetTransform);
	virtual ~Bone() = default;
	uint32_t index = 0;
	XMFLOAT4X4 offsetTransformMatrix = XMFLOAT4X4();
	[[nodiscard]] uint32_t Index() const;
	uint32_t SetIndex(uint32_t newIndex);
	//Row Major Order
	XMMATRIX OffsetTransformMatrix() const;
	XMFLOAT4X4 OffsetTransform() const;
};
