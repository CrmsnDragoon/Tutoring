#pragma once
#include "Model.hpp"
#include "AnimatedMesh.hpp"
#include "AnimationClip.hpp"

class AnimatedMesh;
struct AnimationClip;
class AnimatedModel : public Model {
private:
protected:
public:
	std::vector<AnimatedMesh> meshes{};
	//std::vector<BoneAnimation> boneAnimations{};
	std::vector<AnimationClip> animations{};
	std::vector<Bone*> bones{};
	SceneNode *rootNode = nullptr;
	static AnimatedModel ImportModel(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
	static void ImportAnimation(const char* fileName, const bool flipUVs, const bool flipWindingOrder, AnimatedModel& model);
	static AnimatedModel ImportModel(const uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
	static AnimatedModel ImportModel_FBX(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
	static AnimatedModel ImportModel_FBX(const uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
	
	void setupGL() {
		for (auto& mesh : meshes) {
			mesh.setupGL();
		}
	}
	void Draw() {
		for (auto& mesh : meshes) {
			mesh.Draw();
		}
	}
};

