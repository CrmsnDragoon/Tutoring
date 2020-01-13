
#include "AnimationClip.hpp"


uint32_t BoneAnimation::FindKeyframeIndex(const float time) const {

	if (time <= keyframes.front().time) return 0;
	if (time >= keyframes.back().time) return uint32_t(keyframes.size() - 1);

	uint32_t keyframe_index = 1;
	for (; keyframe_index < keyframes.size() - 1 &&
		   time >= keyframes[keyframe_index].time; ++keyframe_index) {}

	return keyframe_index -1;
}

uint32_t BoneAnimation::GetTransform(float time, XMFLOAT4X4& transform) const {
	const uint32_t keyframe_index = FindKeyframeIndex(time);
	XMStoreFloat4x4(&transform, keyframes[keyframe_index].Transform());
	return keyframe_index;
}

void BoneAnimation::GetInterpolatedTransform(float time, XMFLOAT4X4& transform) const {
	const Keyframe* first_keyframe = &keyframes.front();
	const Keyframe* last_keyframe = &keyframes.back();

	if (first_keyframe->time >= time) {
		XMStoreFloat4x4(&transform, first_keyframe->Transform());
	}
	else if (last_keyframe->time <= time) {
		XMStoreFloat4x4(&transform, last_keyframe->Transform());
	}
	else {
		const size_t keyframe_index = FindKeyframeIndex(time);
		const Keyframe* early_keyframe = &keyframes[keyframe_index];
		const Keyframe* late_keyframe = &keyframes[keyframe_index + 1];

		const float lerpValue = ((time - early_keyframe->time) / (late_keyframe->time - early_keyframe->time));

		const XMVECTOR translation = XMVectorLerp(
			early_keyframe->TranslationVector(),
			late_keyframe->TranslationVector(),
			lerpValue
		);
		const XMVECTOR scale = XMVectorLerp(
			early_keyframe->ScaleVector(), 
			late_keyframe->ScaleVector(), 
			lerpValue
		);
		const XMVECTOR rotation = XMQuaternionSlerp(
			early_keyframe->RotationQuaternionVector(),
			late_keyframe->RotationQuaternionVector(),
			lerpValue
		);

		const XMMATRIX interpolated_mat = XMMatrixAffineTransformation(
			scale, 
			XMVectorZero(), rotation, 
			translation
		);

		XMStoreFloat4x4(&transform, interpolated_mat);
	}
}

void BoneAnimation::SetSceneNodeToInterpolatedTransform(float time, SceneNode* sceneNode) const {
	const Keyframe* first_keyframe = &keyframes.front();
	const Keyframe* last_keyframe = &keyframes.back();

	if (first_keyframe->time <= time) {
		XMStoreFloat3(&sceneNode->position, first_keyframe->TranslationVector());
		XMStoreFloat4(&sceneNode->rotation, first_keyframe->RotationQuaternionVector());
		XMStoreFloat3(&sceneNode->scale, first_keyframe->ScaleVector());
	}
	else if (last_keyframe->time >= time) {
		XMStoreFloat3(&sceneNode->position, last_keyframe->TranslationVector());
		XMStoreFloat4(&sceneNode->rotation, last_keyframe->RotationQuaternionVector());
		XMStoreFloat3(&sceneNode->scale, last_keyframe->ScaleVector());
	}
	else {
		const size_t keyframe_index = FindKeyframeIndex(time);
		const Keyframe* early_keyframe = &keyframes[keyframe_index];
		const Keyframe* late_keyframe = &keyframes[keyframe_index + 1];

		const float lerpValue = ((time - early_keyframe->time) / (late_keyframe->time - early_keyframe->time));

		const XMVECTOR translation = XMVectorLerp(
			early_keyframe->TranslationVector(),
			late_keyframe->TranslationVector(), 
			lerpValue
		);
		const XMVECTOR scale = XMVectorLerp(
			early_keyframe->ScaleVector(),
			late_keyframe->ScaleVector(),
			lerpValue
		);
		const XMVECTOR rotation = XMQuaternionSlerp(
			early_keyframe->RotationQuaternionVector(),
			late_keyframe->RotationQuaternionVector(),
			lerpValue
		);

		XMStoreFloat3(&sceneNode->position, translation);
		XMStoreFloat4(&sceneNode->rotation, rotation);
		XMStoreFloat3(&sceneNode->scale, scale);
		sceneNode->UpdateTransform();
	}
}

/*void BoneAnimation::ImportBoneAnimationChannel(aiNodeAnim* channel) {
	assert(channel->mNumPositionKeys == channel->mNumRotationKeys);
	assert(channel->mNumPositionKeys == channel->mNumScalingKeys);

	keyframes = std::vector<Keyframe>(channel->mNumPositionKeys);

	for (uint32_t key_index = 0; key_index < channel->mNumPositionKeys; ++key_index) {
		const auto pos_key = channel->mPositionKeys[key_index];
		const auto rotation_key = channel->mRotationKeys[key_index];
		const auto scale_key = channel->mScalingKeys[key_index];

		assert(pos_key.mTime == rotation_key.mTime);
		assert(pos_key.mTime == scale_key.mTime);

		keyframes[key_index].translation = XMFLOAT3(
			pos_key.mValue.x,
			pos_key.mValue.y,
			pos_key.mValue.z
		);

		keyframes[key_index].rotation = XMFLOAT4(
			rotation_key.mValue.x,
			rotation_key.mValue.y,
			rotation_key.mValue.z,
			rotation_key.mValue.w
		);

		keyframes[key_index].translation = XMFLOAT3(
			scale_key.mValue.x,
			scale_key.mValue.y,
			scale_key.mValue.z
		);
	}
}*/

void AnimationClip::GetTransform(float currentPlaybackTimeInTicks, SceneNode* sceneNode,
														XMFLOAT4X4& toParentTransform) {
	if (sceneNodeToAnimationMap.contains(sceneNode->name)) {
		sceneNodeToAnimationMap.at(sceneNode->name).GetTransform(currentPlaybackTimeInTicks, toParentTransform);
	}
	else {
		toParentTransform = sceneNode->LocalTransform();
	}
}
void AnimationClip::GetInterpolatedTransform(float currentPlaybackTimeInTicks, SceneNode* sceneNode,
														XMFLOAT4X4& toParentTransform) {
	if (sceneNodeToAnimationMap.contains(sceneNode->name)) {
		sceneNodeToAnimationMap.at(sceneNode->name).GetInterpolatedTransform(currentPlaybackTimeInTicks, toParentTransform);
	}
	else {
		toParentTransform = sceneNode->LocalTransform();
	}
}

void AnimationClip::GetInterpolatedTransform(float currentPlaybackTimeInTicks, SceneNode* sceneNode) {
	if (sceneNodeToAnimationMap.contains(sceneNode->name)) {
		sceneNodeToAnimationMap.at(sceneNode->name).SetSceneNodeToInterpolatedTransform(currentPlaybackTimeInTicks, sceneNode);
	}
}

/*
void AnimationClip::GetTransform(float currentPlaybackTimeInTicks, Bone* bone,
														XMFLOAT4X4& toParentTransform) {
	if (boneToAnimationMap.contains(bone->Index())) {
		boneToAnimationMap.at(bone->Index()).GetTransform(currentPlaybackTimeInTicks, toParentTransform);
		return;
	}
}
void AnimationClip::GetInterpolatedTransform(float currentPlaybackTimeInTicks, Bone* bone,
														XMFLOAT4X4& toParentTransform) {
	if (boneToAnimationMap.contains(bone->Index())) {
		boneToAnimationMap.at(bone->Index()).GetInterpolatedTransform(currentPlaybackTimeInTicks, toParentTransform);
	}
	else {
		toParentTransform = bone->LocalTransform();
	}
}
void AnimationClip::GetInterpolatedTransform(float currentPlaybackTimeInTicks, Bone* bone) {
	if (boneToAnimationMap.contains(bone->Index())) {
		auto& anim = boneToAnimationMap.at(bone->Index());
		anim.SetSceneNodeToInterpolatedTransform(currentPlaybackTimeInTicks, bone);
		return;
	}
	XMStoreFloat3(&bone->position, XMVectorZero());
	XMStoreFloat4(&bone->rotation, XMQuaternionIdentity());
	XMStoreFloat3(&bone->scale, XMVectorSplatOne());

}*/
