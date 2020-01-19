#pragma once
#include "AnimatedModel.hpp"
#include <vector>
#include <string>

struct AnimationClip;

namespace Engine::Graphics {
	struct LoadedMesh;
	class GameplayModel : public AnimatedModel {
	public:
		std::string name;
		//Root of model, skeleton is attached, meshes are offset from this node.
		/*Caconym3D::SceneNode* rootNode = nullptr;*/
		std::vector<Engine::Graphics::LoadedMesh> meshes = std::vector<Engine::Graphics::LoadedMesh>(0);
		/*std::vector<Caconym3D::Bone*> bones = std::vector<Caconym3D::Bone*>(0);
		std::vector<Caconym3D::AnimationClip> animations;
		*/
		[[nodiscard]] size_t MeshCount() const;
		[[nodiscard]] size_t BoneCount() const;
	};
}
