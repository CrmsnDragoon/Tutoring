#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

using namespace DirectX;

 
class SceneNode {
public:
	SceneNode() = default;
	explicit SceneNode(std::string name):name(std::move(name)){}
	virtual ~SceneNode() = default;
	SceneNode* parent = nullptr;
	std::vector<SceneNode*> childList;
	XMFLOAT3 position = XMFLOAT3();
	XMFLOAT4 rotation = XMFLOAT4(0, 0, 0, 1);
	XMFLOAT3 scale = XMFLOAT3(1, 1, 1);
	void UpdateTransform();
	XMFLOAT3 RelativePosition() const;
	virtual SceneNode* Search(const std::string& targetName);
	XMFLOAT4X4 transform = XMFLOAT4X4(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);
	XMFLOAT4X4 inverse = XMFLOAT4X4(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);
	std::string name = std::string();
	[[nodiscard]] XMFLOAT3 Position() const;

	[[nodiscard]] XMVECTOR PositionVector() const;

	[[nodiscard]] XMFLOAT4 Rotation() const;

	[[nodiscard]] XMVECTOR RotationQuaternion() const;

	[[nodiscard]] XMFLOAT3 Scale() const;

	[[nodiscard]] XMVECTOR ScaleVector() const;
	[[nodiscard]] XMMATRIX LocalTransformMatrix() const;
	[[nodiscard]] XMMATRIX TransformMatrix() const;
	[[nodiscard]] XMFLOAT4X4 LocalTransform() const;
	[[nodiscard]] XMFLOAT4X4 Transform() const;
	[[nodiscard]] XMMATRIX InverseLocalTransformMatrix() const;
	[[nodiscard]] XMMATRIX InverseTransformMatrix() const;
	[[nodiscard]] XMFLOAT4X4 InverseLocalTransform() const;
	[[nodiscard]] XMFLOAT4X4 InverseTransform() const;
};
