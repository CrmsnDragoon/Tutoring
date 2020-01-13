
#include "AnimationPlayer.hpp"
#include <cstdint>

bool AnimationPlayer::IsPlayingClip(const AnimationPlayerState& state) {
	return state.model != nullptr &&
		state.currentClipIndex >= 0 &&
		state.currentPlaybackTimeInTicks <= state.model->animations[state.currentClipIndex].duration;
}

bool AnimationPlayer::IsLooping(const AnimationPlayerState& state) { return state.looping; }

float AnimationPlayer::GetCurrentTick(const AnimationPlayerState& state) {
	return state.currentPlaybackTimeInTicks;
}

void AnimationPlayer::SetModel(AnimationPlayerState& state, AnimatedModel* model) { state.model = model; }

void AnimationPlayer::PlayClip(AnimationPlayerState& state, bool looping, uint32_t newClipIndex) {
	const uint32_t num_clips = GetNumberOfClips(state);
	if (num_clips == 0) {
		state.currentClipIndex = -1;
		state.currentPlaybackTimeInTicks = 0;
		state.looping = false;
		return;
	}
	if (num_clips <= newClipIndex) newClipIndex = num_clips - 1;
	
	state.currentClipIndex = newClipIndex;
	state.currentPlaybackTimeInTicks = 0;
	state.looping = looping;
	GetBindPose(state,state.model->rootNode);
}

bool AnimationPlayer::IsAnimationInterpolated(AnimationPlayerState& state) {
	return state.interpolation;
}

void AnimationPlayer::SetAnimationInterpolation(AnimationPlayerState& state, bool interpolated) {
	state.interpolation = interpolated;
}

void AnimationPlayer::GetBindPose(AnimationPlayerState& state, SceneNode* sceneNode) {
	if (sceneNode == nullptr) return;
	XMMATRIX toRootTransform = sceneNode->LocalTransformMatrix();
	SceneNode* parentNode = sceneNode->parent;
	while (parentNode != nullptr) {
		toRootTransform = toRootTransform * parentNode->LocalTransformMatrix();
		parentNode = parentNode->parent;
	}
	Bone* bone = dynamic_cast<Bone*>(sceneNode);
	if (bone != nullptr) {
		XMStoreFloat4x4(&state.boneMats[bone->index],
			bone->OffsetTransformMatrix() * 
			toRootTransform * 
			XMLoadFloat4x4(&state.inverseRootTransform)
		);
	}

	for (auto child : sceneNode->childList) { GetBindPose(state, child); }
}

void AnimationPlayer::Update(const float dt, AnimationPlayerState* states, size_t stateCount) {
/* stateCount and state_index need to both be signed ints
 * uses the entire CPU but ends up being slower.
#pragma omp parallel
#pragma omp for*/
	for (size_t state_index = 0; state_index < stateCount; ++state_index) {
		auto& state = states[state_index];

		//Update root transform
		XMStoreFloat4x4(&state.inverseRootTransform, state.model->rootNode->InverseLocalTransformMatrix());

		if (!IsPlayingClip(state)) continue;

		
		state.currentPlaybackTimeInTicks += dt * state.model->animations[state.currentClipIndex].ticksPerSecond;

		if (state.currentPlaybackTimeInTicks >= state.model->animations[state.currentClipIndex].duration) {
			if (state.looping) {
				while (state.currentPlaybackTimeInTicks > state.model->animations[state.currentClipIndex].duration) {
					state.currentPlaybackTimeInTicks -= state.model->animations[state.currentClipIndex].duration;
				}
			}
			else {
				state.lastClip = &state.model->animations[state.currentClipIndex];
				state.currentClipIndex = -1;
				continue;
			}
		}
		SceneNode* node = state.model->rootNode->parent;
		state.model->rootNode->parent = nullptr;
		if (state.interpolation)
			GetInterpolatedPose(state, state.model->rootNode, XMMatrixIdentity());
		else
			GetPose(state,state.model->rootNode, XMMatrixIdentity());
		state.model->rootNode->parent = node;
	}
}

void AnimationPlayer::GetPose(AnimationPlayerState& state, SceneNode* sceneNode, const XMMATRIX parentTransform) {
	if (sceneNode == nullptr) return;
	
	XMFLOAT4X4 toParentTransform = XMFLOAT4X4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	Bone* bone = dynamic_cast<Bone*>(sceneNode);
	if (bone != nullptr) {
		state.model->animations[state.currentClipIndex].GetTransform(state.currentPlaybackTimeInTicks, bone, toParentTransform);
	}
	else {
		state.model->animations[state.currentClipIndex].GetTransform(state.currentPlaybackTimeInTicks, sceneNode, toParentTransform);
	}
	const XMMATRIX toRootTransform = XMLoadFloat4x4(&toParentTransform) * parentTransform;

	XMStoreFloat4x4(&state.mToRootTransforms[sceneNode], toRootTransform);
	if (bone != nullptr) {
		XMStoreFloat4x4(
			&state.boneMats[bone->Index()],
			bone->OffsetTransformMatrix() * 
				toRootTransform * 
				XMLoadFloat4x4(&state.inverseRootTransform));
	}

	XMStoreFloat4x4(&state.mToRootTransforms[sceneNode], toRootTransform);
	for (SceneNode* child : sceneNode->childList) {
		GetPose(state, child, toRootTransform);
	}
}

void AnimationPlayer::GetInterpolatedPose(AnimationPlayerState& state, SceneNode* sceneNode, const XMMATRIX parentTransform) {
	if (sceneNode == nullptr) return;
	/*Bone* bone = dynamic_cast<Bone*>(sceneNode);
	if (bone != nullptr) {
		state.model->animations[state.currentClipIndex].
			GetInterpolatedTransform(state.currentPlaybackTimeInTicks, bone);
	}
	const XMMATRIX toRootTransform = sceneNode->TransformMatrix();
	if (bone != nullptr) {
		XMStoreFloat4x4(&state.boneMats[bone->Index()],
						bone->OffsetTransformMatrix() * toRootTransform * XMLoadFloat4x4(&state.inverseRootTransform));
	}
	for (SceneNode* child : sceneNode->childList) { GetInterpolatedPose(state, child); }
	*/
	//As long as GetInterpolatedTransform modifies the scene node's local matrix, the above does the same as the below code.
	
	XMFLOAT4X4 toParentTransform = XMFLOAT4X4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	Bone* bone = dynamic_cast<Bone*>(sceneNode);
	if (bone != nullptr) {
		state.model->animations[state.currentClipIndex].GetInterpolatedTransform(state.currentPlaybackTimeInTicks, bone, toParentTransform);
	}
	else {
		state.model->animations[state.currentClipIndex].GetInterpolatedTransform(state.currentPlaybackTimeInTicks, sceneNode, toParentTransform);
	}

	const XMMATRIX toRootTransform = XMLoadFloat4x4(&toParentTransform) * parentTransform;

	XMStoreFloat4x4(&state.mToRootTransforms[sceneNode], toRootTransform);


	if (bone != nullptr) {
		XMStoreFloat4x4(
			&state.boneMats[bone->Index()],
			bone->OffsetTransformMatrix() * 
				toRootTransform * 
				XMLoadFloat4x4(&state.inverseRootTransform));
	}
	for (SceneNode* child : sceneNode->childList) {
		GetInterpolatedPose(state, child, toRootTransform);
	}
}

uint32_t AnimationPlayer::GetNumberOfClips(const AnimationPlayerState& animationPlayerState) {
	return uint32_t(animationPlayerState.model->animations.size());
}
