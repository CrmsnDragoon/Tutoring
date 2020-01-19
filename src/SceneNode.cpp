#include "SceneNode.hpp"
#include "AnimatedMesh.hpp"

void SceneNode::UpdateTransform() {
	const XMVECTOR pos = XMLoadFloat3(&position),
	rot = XMLoadFloat4(&this->rotation),
	loc_scale = XMLoadFloat3(&this->scale);
	XMMATRIX parentTransform = XMMatrixIdentity();
	if (parent != nullptr) {
		parentTransform = parent->TransformMatrix();
	}

	const XMMATRIX locTransform =  XMMatrixAffineTransformation(loc_scale, XMVectorZero(),rot, pos);
	XMStoreFloat4x4(&transform,locTransform * parentTransform);
	const auto inverseLoc = XMMatrixInverse(nullptr,locTransform);
	XMStoreFloat4x4(&inverse,inverseLoc);
	
	for (size_t child_index = 0; child_index < childList.size(); ++child_index) {
		childList[child_index]->UpdateTransform();
	}
}

XMFLOAT3 SceneNode::RelativePosition() const {
	XMVECTOR pos = XMLoadFloat3(&position);
	
	XMMATRIX parentTransform = XMMatrixIdentity();
	if (parent != nullptr) {
		parentTransform = XMLoadFloat4x4(&parent->transform);
	}
	pos = XMVector3Transform(pos,parentTransform);
	XMFLOAT3 rel_pos{};
	XMStoreFloat3(&rel_pos, pos);
	return rel_pos;
}

SceneNode* SceneNode::Search(const std::string& targetName) {
	if (name == targetName) return this;
	for (size_t i = 0; i < childList.size(); i++) {
		SceneNode* out = childList[i]->Search(targetName);
		if (out != nullptr) return out;
	}
	return nullptr;
}

XMFLOAT3 SceneNode::Position() const {
	return position;
}

XMVECTOR SceneNode::PositionVector() const {
	return XMLoadFloat3(&position);
}

XMFLOAT4 SceneNode::Rotation() const {
	return rotation;
}

XMVECTOR SceneNode::RotationQuaternion() const {
	return XMLoadFloat4(&rotation);
}

XMFLOAT3 SceneNode::Scale() const {
	return scale;
}

XMVECTOR SceneNode::ScaleVector() const {
	return XMLoadFloat3(&scale);
}

XMMATRIX SceneNode::TransformMatrix() const{
	return XMLoadFloat4x4(&transform);
}
XMMATRIX SceneNode::LocalTransformMatrix() const{
	const XMVECTOR pos = XMLoadFloat3(&this->position);
	const XMVECTOR loc_scale = XMLoadFloat3(&this->scale);
	const XMVECTOR rot = XMLoadFloat4(&this->rotation);
	const XMMATRIX locTransform =  XMMatrixAffineTransformation(loc_scale, XMVectorZero(),rot, pos);
	return locTransform;
}
XMFLOAT4X4 SceneNode::Transform() const{
	return transform;
}
XMFLOAT4X4 SceneNode::LocalTransform() const{
	XMFLOAT4X4 localTransform{};
	const XMVECTOR pos = XMLoadFloat3(&this->position);
	const XMVECTOR rot = XMLoadFloat4(&this->rotation);
	const XMVECTOR loc_scale = XMLoadFloat3(&this->scale);
	const XMMATRIX locTransform =  XMMatrixAffineTransformation(loc_scale, XMVectorZero(),rot, pos);
	XMStoreFloat4x4(&localTransform,locTransform);
	return localTransform;
}

XMMATRIX SceneNode::InverseLocalTransformMatrix() const {
	const XMVECTOR pos = XMLoadFloat3(&this->position);
	const XMVECTOR rot = XMLoadFloat4(&this->rotation);
	const XMVECTOR loc_scale = XMLoadFloat3(&this->scale);
	XMMATRIX locTransform =  XMMatrixAffineTransformation(loc_scale, XMVectorZero(),rot, pos);
	locTransform = XMMatrixInverse(nullptr,locTransform);
	return locTransform;
}

XMMATRIX SceneNode::InverseTransformMatrix() const {
	return XMLoadFloat4x4(&inverse);
}
XMFLOAT4X4 SceneNode::InverseLocalTransform() const {
	XMFLOAT4X4 localTransform{};
	const XMVECTOR pos = XMLoadFloat3(&this->position);
	const XMVECTOR rot = XMLoadFloat4(&this->rotation);
	const XMVECTOR loc_scale = XMLoadFloat3(&this->scale);
	XMMATRIX locTransform =  XMMatrixAffineTransformation(loc_scale, XMVectorZero(),rot, pos);
	locTransform = XMMatrixInverse(nullptr,locTransform);
	XMStoreFloat4x4(&localTransform,locTransform);
	return localTransform;
}
XMFLOAT4X4 SceneNode::InverseTransform() const{
	return inverse;
}