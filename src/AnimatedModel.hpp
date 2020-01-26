#pragma once
#include "Model.hpp"
#include "AnimatedMesh.hpp"
#include "AnimationClip.hpp"

class AnimatedModel : public Model {
private:
protected:
public:
	std::vector<AnimatedMesh> meshes{};
	std::vector<AnimationClip> animations{};
	std::vector<Bone*> bones{};
	SceneNode *rootNode = nullptr;
	std::vector<Material> materials;
	static AnimatedModel ImportModel(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
	static void ImportAnimation(const char* fileName, const bool flipUVs, const bool flipWindingOrder, AnimatedModel& model);
	static AnimatedModel ImportModel(const uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
	
	void setupGL() {
		for (auto& mesh : meshes) {
			mesh.setupGL();
		}
	}
	void Draw1_0() {
		for (auto& mesh : meshes) {
			mesh.Draw1_0();
		}
	}
	void Draw3_2() {
		for (auto& mesh : meshes) {
			mesh.Draw3_2();
		}
	}

	void Shutdown() {
		for (auto& mesh : meshes) {
			mesh.Shutdown();
		}
	}
};

