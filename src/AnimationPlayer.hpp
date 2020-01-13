#pragma once
#include "Util.hpp"
#include "Model.hpp"
#include <DirectXMath.h>
#include "AnimatedModel.hpp"

using namespace DirectX;

	struct AnimationPlayer {
		//Opaque animation state
		class AnimationPlayerState {
			friend AnimationPlayer;
		protected:
			AnimatedModel* model = nullptr;
			float currentPlaybackTimeInTicks = 0;
			int currentClipIndex = -1; // if -1 not playing a clip
			bool looping = false;
			bool interpolation = true;
			AnimationClip* lastClip = nullptr; // for interpolating between animations
			XMFLOAT4X4 rootTransform = XMFLOAT4X4(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 );
			XMFLOAT4X4 inverseRootTransform = XMFLOAT4X4(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 );
		public:
			std::vector<XMFLOAT4X4> boneMats = std::vector<XMFLOAT4X4>(256,
																	   DirectX::XMFLOAT4X4(
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 ));

			std::map<SceneNode*, XMFLOAT4X4> mToRootTransforms;
		};

		[[nodiscard]] static bool IsPlayingClip(const AnimationPlayerState& state);
		[[nodiscard]] static bool IsLooping(const AnimationPlayerState& state);
		[[nodiscard]] static float GetCurrentTick(const AnimationPlayerState& state);

		static void SetModel(AnimationPlayerState& state, AnimatedModel* model);
		static void PlayClip(AnimationPlayerState& state, bool looping = false, uint32_t newClipIndex = 0);
		static bool IsAnimationInterpolated(AnimationPlayerState& state);
		static void SetAnimationInterpolation(AnimationPlayerState& state, bool interpolated);
		static void GetBindPose(AnimationPlayerState& state, SceneNode* sceneNode);
		static void Update(float dt, AnimationPlayerState* states, size_t stateCount);

		static void GetPose(AnimationPlayerState& state, SceneNode* sceneNode, const XMMATRIX parentTransform);
		static void GetInterpolatedPose(AnimationPlayerState& state, SceneNode* sceneNode, const XMMATRIX parentTransform);
		static uint32_t GetNumberOfClips(const AnimationPlayerState& animationPlayerState);
	};
