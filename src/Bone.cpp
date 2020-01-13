
#include "Bone.hpp"

Bone::Bone(const std::string& name, uint32_t index, const XMFLOAT4X4& offsetTransform) {
	this->name = name;
	this->index = index;
	this->offsetTransformMatrix = offsetTransform;
}

uint32_t Bone::Index() const { return index; }

uint32_t Bone::SetIndex(const uint32_t newIndex) {
	index = newIndex;
	return index;
}

XMMATRIX Bone::OffsetTransformMatrix() const { return XMLoadFloat4x4(&offsetTransformMatrix); }
XMFLOAT4X4 Bone::OffsetTransform() const { return offsetTransformMatrix; }
