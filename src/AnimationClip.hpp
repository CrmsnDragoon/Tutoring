#pragma once
#include <map>
#include <string>
#include <vector>
#include "SceneNode.hpp"
#include <DirectXMath.h>

using namespace DirectX;

class Bone;
struct Keyframe {
	float time;
	XMFLOAT3 translation = {0,0,0};
	XMFLOAT4 rotation = {0,0,0,1};
	XMFLOAT3 scale = {1,1,1};

	[[nodiscard]] XMVECTOR TranslationVector() const { return XMLoadFloat3(&translation);}
	[[nodiscard]] XMVECTOR RotationQuaternionVector() const { return XMLoadFloat4(&rotation);}
	[[nodiscard]] XMVECTOR ScaleVector() const { return XMLoadFloat3(&scale);}
	
	[[nodiscard]] XMMATRIX Transform() const {
		return XMMatrixAffineTransformation(ScaleVector(), XMVectorZero(), RotationQuaternionVector(), TranslationVector());
	}
};
struct BoneAnimation {
	friend struct AnimationClip;
	
	std::string nodeName;
	std::vector<Keyframe> keyframes;

	[[nodiscard]] uint32_t FindKeyframeIndex(const float time) const;

	uint32_t GetTransform(float time, XMFLOAT4X4& transform) const;

	void GetInterpolatedTransform(float time, XMFLOAT4X4& transform) const;

	void SetSceneNodeToInterpolatedTransform(float time, SceneNode* sceneNode) const;

	/*void ImportBoneAnimationChannel(aiNodeAnim* channel);*/
};
struct AnimationClip {
	std::string name;
	float duration = 0; // In ticks
	float ticksPerSecond = -1; //Keyframes per second
	/*std::map<uint32_t,BoneAnimation> boneToAnimationMap{};*/
	std::map<std::string,BoneAnimation> sceneNodeToAnimationMap{};
	void GetTransform(float currentPlaybackTimeInTicks, SceneNode* sceneNode, XMFLOAT4X4& toParentTransform);
	void GetInterpolatedTransform(float currentPlaybackTimeInTicks, SceneNode* sceneNode, XMFLOAT4X4& toParentTransform);
	//Modify the bone's components (and therefore transform) to match the most recent transform.
	void GetInterpolatedTransform(float currentPlaybackTimeInTicks, SceneNode* sceneNode);
	/*void GetTransform(float currentPlaybackTimeInTicks, Bone* bone, XMFLOAT4X4& toParentTransform);
	void GetInterpolatedTransform(float currentPlaybackTimeInTicks, Bone* bone, XMFLOAT4X4& toParentTransform);
	void GetInterpolatedTransform(float currentPlaybackTimeInTicks, Bone* bone);*/
};
