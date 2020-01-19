#include "AnimatedModel.hpp"
#include <list>
#include <set>

#define CACONYM_3D_PRINT_MATERIALS 1
#define CACONYM_3D_PRINT_SKELETON 0
#define CACONYM_3D_PRINT_ANIMATIONS 0


#include <DirectXMath.h>
#include "AnimationUtils.hpp"
#include <Assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Util.hpp"

using namespace DirectX;

SceneNode* AddNode(aiNode* node, SceneNode* parent, AnimatedModel* model) {
	if (node == nullptr) return nullptr;
	SceneNode* scene_node = nullptr;

	bool found_bone = false;
	for (Bone* bone : model->bones) {
		if (strcmp(node->mName.C_Str(),bone->name.c_str()) == 0) {
			scene_node = bone;
			found_bone = true;
			break;
		}
	}

	if (!found_bone) {
		scene_node = new SceneNode(node->mName.C_Str());
		for (auto mesh : model->meshes) {
			if (mesh.sceneNode != nullptr && strcmp(node->mName.C_Str(), mesh.sceneNode->name.c_str()) == 0) {
				delete scene_node;
				scene_node = mesh.sceneNode;
				mesh.sceneNode->name = node->mName.C_Str();
				found_bone = true;
				break;
			}
			else if (mesh.sceneNode == nullptr && strcmp(node->mName.C_Str(), mesh.name.c_str()) == 0) {
				mesh.sceneNode = scene_node;
				break;
			} 
		}
		
	}
	scene_node->parent = parent;
	
	
	aiVector3t<float> scale;
	aiQuaterniont<float> rot;
	aiVector3t<float> pos;
	node->mTransformation.Decompose(scale,rot,pos);
	scene_node->position = XMFLOAT3{pos.x,pos.y,pos.z};
	scene_node->rotation = XMFLOAT4{rot.x,rot.y,rot.z,rot.w};
	scene_node->scale = XMFLOAT3{scale.x,scale.y,scale.z};
	auto mat = XMFLOAT4X4(reinterpret_cast<const float*>(node->mTransformation[0]));
	XMMATRIX transform = XMLoadFloat4x4(&mat);
	transform = XMMatrixTranspose(transform);
	XMStoreFloat4x4(&scene_node->transform,transform);
	for (size_t mesh_index = 0; mesh_index < node->mNumMeshes; ++mesh_index) {
		const auto loaded_mesh_index = node->mMeshes[mesh_index];
		model->meshes[loaded_mesh_index].sceneNode = scene_node;
		//This feels like a hack, but is required for some formats.
		model->meshes[loaded_mesh_index].name = scene_node->name;
	}
	scene_node->childList.resize(node->mNumChildren);
	for (size_t node_index = 0; node_index < node->mNumChildren; node_index++)
	{
		scene_node->childList[node_index] = AddNode(node->mChildren[node_index], scene_node, model);
	}
	return scene_node;
}
void ImportAnimatedMesh(AnimatedMesh& out, const aiMesh* mesh, std::map<std::string,uint32_t>& boneNameToIndex, std::set<uint32_t>& usedIndexes) {
	if (mesh->HasBones()) {
		out.bones.reserve(mesh->mNumBones);
		std::vector<BoneWeight_interstitial> bone_weights_vertex_based(mesh->mNumVertices);
		for (uint32_t bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
			XMFLOAT4X4 temp_mat = XMFLOAT4X4(reinterpret_cast<const float*>(mesh->mBones[bone_index]->mOffsetMatrix[0]));
			XMMATRIX offset_matrix = XMLoadFloat4x4(&temp_mat);
			offset_matrix = XMMatrixTranspose(offset_matrix);
			XMStoreFloat4x4(&temp_mat, offset_matrix);
			const auto current_bone = mesh->mBones[bone_index];

			uint32_t bone_index_to_save = bone_index;

			if (boneNameToIndex.contains(current_bone->mName.C_Str())) {
				bone_index_to_save = boneNameToIndex.at(current_bone->mName.C_Str());
			}
			else {
				if (usedIndexes.contains(bone_index)) {
					Utils::PlatformTextOut("Index Conflict for bone: %s! \n", current_bone->mName.C_Str());
					bone_index_to_save = 0;
					while(usedIndexes.contains(bone_index_to_save)) {
						bone_index_to_save++;
					}
					Utils::PlatformTextOut("\told index: %d new index: %d \n", bone_index, bone_index_to_save);
				}
				boneNameToIndex.insert_or_assign(current_bone->mName.C_Str(), bone_index_to_save);
				usedIndexes.emplace(bone_index_to_save);
			}
			
			Bone *bone = new Bone(current_bone->mName.C_Str(), bone_index_to_save, temp_mat);
			out.bones.emplace_back(bone);
			const auto num_weights = mesh->mBones[bone_index]->mNumWeights;
			
			//Bone based boneWeights to vertex based boneWeights
			for (uint32_t weight_index = 0; weight_index < num_weights; ++weight_index) {
				const uint32_t current_index = current_bone->mWeights[weight_index].mVertexId;
				while (bone_weights_vertex_based.size() < current_index) 
					bone_weights_vertex_based.emplace_back();
				auto weight_to_test = current_bone->mWeights[weight_index].mWeight;

				bone_weights_vertex_based[current_index].boneNames.emplace_back(mesh->mBones[bone_index]->mName.C_Str());
				bone_weights_vertex_based[current_index].weights.emplace_back(weight_to_test);
				bone_weights_vertex_based[current_index].indexes.emplace_back(bone_index_to_save);
				assert(bone_weights_vertex_based[current_index].indexes.size() == bone_weights_vertex_based[current_index].weights.size());
			}
		}
		out.boneWeights = std::vector<BoneWeight>(mesh->mNumVertices);
		//Compress boneWeights down to runtime format (4 boneWeights per vertex max)
		for (uint32_t vertex_index = 0; vertex_index < mesh->mNumVertices; ++vertex_index) {
			XMVECTOR index_temp = XMVectorSetIntByIndex(XMVECTOR{},0,0);
			XMVECTOR weight_temp = XMVectorZero();
			const auto weight_count = bone_weights_vertex_based[vertex_index].indexes.size();
			uint32_t weight_index = 0;
			for (; weight_index < 4 && weight_index < weight_count; ++weight_index) {
				index_temp = XMVectorSetIntByIndex(index_temp, bone_weights_vertex_based[vertex_index].indexes[weight_index], weight_index);
				weight_temp = XMVectorSetByIndex(weight_temp, bone_weights_vertex_based[vertex_index].weights[weight_index], weight_index);
			}
			//Ensure that any unassigned boneWeights are zero
			for (; weight_index < 4; ++weight_index) {
				index_temp.m128_u32[weight_index] = 0;
				weight_temp.m128_f32[weight_index] = 0;
			}
			
			//weight_temp = XMVector4Normalize(weight_temp); // may not be needed, but in case that there's more than 4 bones influencing this vert...
			out.boneWeights[vertex_index].boneIndexes.x = index_temp.m128_u32[0];
			out.boneWeights[vertex_index].boneIndexes.y = index_temp.m128_u32[1];
			out.boneWeights[vertex_index].boneIndexes.z = index_temp.m128_u32[2];
			out.boneWeights[vertex_index].boneIndexes.w = index_temp.m128_u32[3];
			assert(out.boneWeights[vertex_index].boneIndexes.x == index_temp.m128_u32[0]);
			assert(out.boneWeights[vertex_index].boneIndexes.y == index_temp.m128_u32[1]);
			assert(out.boneWeights[vertex_index].boneIndexes.z == index_temp.m128_u32[2]);
			assert(out.boneWeights[vertex_index].boneIndexes.w == index_temp.m128_u32[3]);
			XMStoreFloat4(&out.boneWeights[vertex_index].boneWeights, weight_temp);
			assert(out.boneWeights[vertex_index].boneWeights.x == weight_temp.m128_f32[0]);
			assert(out.boneWeights[vertex_index].boneWeights.y == weight_temp.m128_f32[1]);
			assert(out.boneWeights[vertex_index].boneWeights.z == weight_temp.m128_f32[2]);
			assert(out.boneWeights[vertex_index].boneWeights.w == weight_temp.m128_f32[3]);
		}
	}
}
void ImportBoneAnimationChannel_CombinedChannelsOnly(BoneAnimation& anim, aiNodeAnim* channel) {
	assert(channel->mNumPositionKeys == channel->mNumRotationKeys);
	assert(channel->mNumPositionKeys == channel->mNumScalingKeys);
	
	anim.keyframes = std::vector<Keyframe>(channel->mNumPositionKeys);

	for (uint32_t key_index = 0; key_index < channel->mNumPositionKeys; ++key_index) {
		const auto pos_key = channel->mPositionKeys[key_index];
		const auto rotation_key = channel->mRotationKeys[key_index];
		const auto scale_key = channel->mScalingKeys[key_index];

		assert(pos_key.mTime == rotation_key.mTime);
		assert(pos_key.mTime == scale_key.mTime);

		anim.keyframes[key_index].time = float(pos_key.mTime);
		
		anim.keyframes[key_index].translation = XMFLOAT3(
			pos_key.mValue.x,
			pos_key.mValue.y,
			pos_key.mValue.z
		);

		anim.keyframes[key_index].rotation = XMFLOAT4(
			rotation_key.mValue.x,
			rotation_key.mValue.y,
			rotation_key.mValue.z,
			rotation_key.mValue.w
		);

		anim.keyframes[key_index].scale = XMFLOAT3(
			scale_key.mValue.x,
			scale_key.mValue.y,
			scale_key.mValue.z
		);
	}
}
void InsertMiddleAnimationFrame(BoneAnimation& anim, const float time, size_t& keyframe_index) {
	keyframe_index = 0;
			
	if (anim.keyframes.size() == 0){
		anim.keyframes.emplace_back();
		keyframe_index = 0;
	}
	else {
		auto iter = anim.keyframes.begin();
		while(iter != anim.keyframes.end() && iter->time < time) {
			++iter;
			keyframe_index++;
		}
				
		if (iter == anim.keyframes.begin()) {
			++iter;
			anim.keyframes.insert(iter,anim.keyframes[0]);
			keyframe_index = 1;
		}
		else if (iter == anim.keyframes.end()) {
			anim.keyframes.emplace_back(anim.keyframes[anim.keyframes.size()-1]);
			keyframe_index = anim.keyframes.size()-1;
		}
		else {
			const auto last_frame = --iter;
			++iter;

			XMVECTOR pos, scale, rot;
			const float lerpValue = (time - iter->time) / (last_frame->time - iter->time);
			XMVECTOR pre_pos = XMLoadFloat3(&last_frame->translation);
			XMVECTOR post_pos = XMLoadFloat3(&iter->translation);
			pos = XMVectorLerp(pre_pos,post_pos,lerpValue);
			XMVECTOR pre_scale = XMLoadFloat3(&last_frame->scale);
			XMVECTOR post_scale = XMLoadFloat3(&iter->scale);
			scale = XMVectorLerp(pre_scale,post_scale,lerpValue);
			XMVECTOR pre_rot = last_frame->RotationQuaternionVector();
			XMVECTOR post_rot = iter->RotationQuaternionVector();
			rot = XMQuaternionSlerp(pre_rot,post_rot,lerpValue);

			Keyframe newFrame{};
			XMStoreFloat3(&newFrame.translation,pos);
			XMStoreFloat3(&newFrame.scale,scale);
			XMStoreFloat4(&newFrame.rotation,rot);
					
			anim.keyframes.insert(iter,newFrame);
			keyframe_index++;
		}
	}
	anim.keyframes[keyframe_index].time = time;
}
void ImportBoneAnimationChannel(BoneAnimation& anim, aiNodeAnim* channel) {
//	anim.keyframes = std::vector<Keyframe>(channel->mNumPositionKeys);
	anim.nodeName = channel->mNodeName.C_Str();
	for (uint32_t key_index = 0; key_index < channel->mNumPositionKeys; ++key_index) {
		const auto pos_key = channel->mPositionKeys[key_index];
		const float time = float(pos_key.mTime);
		
		size_t keyframe_index = 0;
		for (auto  keyframe: anim.keyframes) {
			if (pos_key.mTime == keyframe.time) {
				break;
			}
			keyframe_index++;
		}
		
		if (keyframe_index >= anim.keyframes.size()) {
			InsertMiddleAnimationFrame(anim, time, keyframe_index);
		}
		
		anim.keyframes[keyframe_index].translation = XMFLOAT3(
			pos_key.mValue.x,
			pos_key.mValue.y,
			pos_key.mValue.z
		);
	}
	for (uint32_t key_index = 0; key_index < channel->mNumRotationKeys; ++key_index) {
		const auto rotation_key = channel->mRotationKeys[key_index];
		const float time = float(rotation_key.mTime);
		
		size_t keyframe_index = 0;
		for (auto  keyframe: anim.keyframes) {
			if (time == keyframe.time) {
				break;
			}
			keyframe_index++;
		}

		if (keyframe_index >= anim.keyframes.size()) {
			InsertMiddleAnimationFrame(anim, time, keyframe_index);
		}
		
		anim.keyframes[keyframe_index].rotation = XMFLOAT4(
			rotation_key.mValue.x,
			rotation_key.mValue.y,
			rotation_key.mValue.z,
			rotation_key.mValue.w
		);
	}
	for (uint32_t key_index = 0; key_index < channel->mNumScalingKeys; ++key_index) {
		const auto scale_key = channel->mScalingKeys[key_index];
		const float time = float(scale_key.mTime);
		
		size_t keyframe_index = 0;
		for (auto  keyframe: anim.keyframes) {
			if (time == keyframe.time) {
				break;
			}
			keyframe_index++;
		}

		if (keyframe_index >= anim.keyframes.size()) {
			InsertMiddleAnimationFrame(anim, time, keyframe_index);
		}
		
		anim.keyframes[keyframe_index].scale = XMFLOAT3(
			scale_key.mValue.x,
			scale_key.mValue.y,
			scale_key.mValue.z
		);
	}
}
void AddAnimationToModel(const aiScene* scene, const aiAnimation* anim, AnimatedModel& outModel) {
	const uint32_t animation_index = uint32_t(outModel.animations.size());
	outModel.animations.emplace_back();
	
	PrintAnimation(anim);
	outModel.animations[animation_index].name = anim->mName.C_Str();
	outModel.animations[animation_index].duration = float(anim->mDuration);
	outModel.animations[animation_index].ticksPerSecond = float(anim->mTicksPerSecond);
	for (uint32_t anim_index = 0; anim_index < anim->mNumChannels; ++anim_index) {
		if (!outModel.animations[animation_index].sceneNodeToAnimationMap.contains(std::string(anim->mChannels[anim_index]->mNodeName.C_Str()))) {
			outModel.animations[animation_index].sceneNodeToAnimationMap.insert_or_assign(
				std::string(anim->mChannels[anim_index]->mNodeName.C_Str()), BoneAnimation{});
		}

		ImportBoneAnimationChannel(outModel.animations[animation_index].sceneNodeToAnimationMap.at(
				std::string(anim->mChannels[anim_index]->mNodeName.C_Str())),
								   anim->mChannels[anim_index]);
	}
	for (uint32_t anim_index = 0; anim_index < anim->mNumMeshChannels; ++anim_index) {
		const auto channel = anim->mMeshChannels[anim_index];
		auto name = channel->mName;
		uint32_t mesh_index;
		for (mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
			if (scene->mMeshes[mesh_index]->mName == name) break;
		}
		if (outModel.meshes.size() >= mesh_index) continue;
		const auto num_keys = channel->mNumKeys;
		std::vector<AnimatedMesh> meshes;
		for (uint32_t key_index = 0; key_index < num_keys; ++key_index) {
			channel->mKeys[key_index].mTime;
			const auto anim_mesh_index = channel->mKeys[key_index].mValue;
			const auto anim_mesh = scene->mMeshes[mesh_index]->mAnimMeshes[anim_mesh_index];//these are just meshes with no indices
			//TODO: Vertex animation importing
			anim_mesh->mName;
			anim_mesh->mWeight;
			for (size_t i = 0; i < anim_mesh->mNumVertices; i++) {
				anim_mesh->mVertices;
			}
			for (size_t i = 0; i < anim_mesh->mNumVertices; i++) {
				anim_mesh->mNormals;
			}
			for (size_t i = 0; i < anim_mesh->mNumVertices; i++) {
				anim_mesh->mTangents;
				anim_mesh->mBitangents;
			}
			for (size_t i = 0; i < anim_mesh->mNumVertices; i++) {
				anim_mesh->mTextureCoords;
			}
			for (size_t i = 0; i < anim_mesh->mNumVertices; i++) {
				anim_mesh->mColors;
			}
		}
	}
	for (uint32_t anim_index = 0; anim_index < anim->mNumMorphMeshChannels; ++anim_index) {
		const auto channel = anim->mMorphMeshChannels[anim_index];
		/*const auto name =*/ channel->mName;
		for (size_t key_index = 0; key_index < channel->mNumKeys; key_index++) {
			const auto& key = channel->mKeys[key_index];
			/*const float time =*/ float(key.mTime);
			for (size_t index = 0; index < channel->mKeys[key_index].mNumValuesAndWeights; index++)
			{
				key.mValues[index];
				key.mWeights[index];
			}
		}
	}
}
AnimatedModel ImportAnimatedModelAssimp(const aiScene* const scene) {
	if (scene == nullptr)
		return {};
	AnimatedModel outModel;
	if (scene->HasMeshes()) {
		std::map<std::string,uint32_t> boneNameToIndex;
		std::set<uint32_t> usedIndexes;
		const uint32_t num_meshes = scene->mNumMeshes;
		outModel.meshes.resize(scene->mNumMeshes);
		for (uint32_t mesh_index = 0; mesh_index < num_meshes; ++mesh_index) {
			const auto mesh = scene->mMeshes[mesh_index];
			ImportMesh(outModel.meshes[mesh_index], mesh, scene);
		}
		for (uint32_t mesh_index = 0; mesh_index < num_meshes; ++mesh_index) {
			const auto mesh = scene->mMeshes[mesh_index];
			ImportAnimatedMesh(outModel.meshes[mesh_index], mesh,boneNameToIndex, usedIndexes);

			//Copy bones over
			for(Bone* bone : outModel.meshes[mesh_index].bones) {
				bool found = false;
				for(Bone* bone_find : outModel.bones) {
					if (bone->name == bone_find->name) {
						found = true;
					}
				}
				if (!found && outModel.bones.size() <= bone->Index()) {
					outModel.bones.emplace_back(bone);
				}
				else if (!found) {
					auto temp = outModel.bones[bone->Index()];
					outModel.bones[bone->Index()] = bone;
					outModel.bones.emplace_back(temp);
				}
			}
		}
	}
	const auto bone_length = outModel.bones.size();
	
	for (uint32_t bone_index = 0; bone_index < bone_length; ++bone_index) {
		const auto bone = outModel.bones[bone_index];
		if (bone_index != outModel.bones[bone_index]->Index()) {
			if (bone_length <= bone->Index()) {
				const auto temp = outModel.bones[outModel.bones.size()-1];
				outModel.bones[outModel.bones.size()-1] = bone;
				outModel.bones[bone_index] = temp;
				bone_index--;
			}
			else {
				const auto temp = outModel.bones[bone->Index()];
				outModel.bones[bone->Index()] = bone;
				outModel.bones[bone_index] = temp;
				bone_index--;
			}
		}
	}
	
	if (scene->mRootNode != nullptr) {
		// construct skeleton
		outModel.rootNode = AddNode(scene->mRootNode, nullptr, &outModel);
		PrintSkeleton(outModel.rootNode);
	}
	if (scene->HasAnimations()) {
		for (uint32_t animation_index = 0; animation_index < scene->mNumAnimations; ++animation_index) {
			auto anim = scene->mAnimations[animation_index];
			AddAnimationToModel(scene, anim, outModel);
		}
	}
	if (scene->HasMaterials()) {
		for (size_t material_index = 0; material_index < scene->mNumMaterials; material_index++)
		{
			//TODO(Dragoon): Materials?
			aiMaterial* material = scene->mMaterials[material_index];
			PrintMaterial(material);
		}
	}
	if (scene->HasTextures()) {
		for (size_t texture_index = 0; texture_index < scene->mNumTextures; texture_index++)
		{
			const aiTexture* texture = scene->mTextures[texture_index];
			Utils::PlatformTextOut("\nTexture Filename: %s\n\tWidth:%d\n\tHeight:%d",texture->mFilename.C_Str(),
			texture->mWidth,
			texture->mHeight);
		}
	}
	// We currently don't care about Lights or cameras
	if (scene->HasLights()) {
		for (uint32_t light_index = 0; light_index < scene->mNumLights; ++light_index) {
			scene->mLights[light_index];
		}
	}
	if (scene->HasCameras()) {
		for (uint32_t camera_index = 0; camera_index < scene->mNumCameras; ++camera_index) {
			scene->mCameras[camera_index];
		}
	}
	return outModel;
}
AnimatedModel AnimatedModel::ImportModel(const char* fileName, const bool flipUVs, const bool flipWindingOrder) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(fileName, AssimpImportFlags(flipUVs, flipWindingOrder));
	if (scene == nullptr) {
		Utils::PlatformTextOut("Failed to find model %s, Assimp returned error: %s", fileName, importer.GetErrorString());
		return {};
	}
	return ImportAnimatedModelAssimp(scene);
}
void AnimatedModel::ImportAnimation(const char* fileName, const bool flipUVs, const bool flipWindingOrder, AnimatedModel& model) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(fileName, AssimpImportFlags(flipUVs, flipWindingOrder));
	if (scene == nullptr) {
		Utils::PlatformTextOut("Failed to find model %s, Assimp returned error: %s", fileName, importer.GetErrorString());
		return;
	}
	if (scene->HasAnimations()) {
		for (uint32_t anim_index = 0; anim_index < scene->mNumAnimations; ++anim_index) {
			const aiAnimation* anim = scene->mAnimations[anim_index];
			bool skip = false;
			for (auto clip : model.animations) {
				if (strcmp(anim->mName.C_Str(),clip.name.c_str()) == 0) {
					skip = true;
					break;
				}
			}
			if (skip) continue;
			AddAnimationToModel(scene, anim, model);		
		}
	}
}
AnimatedModel AnimatedModel::ImportModel(const uint8_t* uint8_tArray, const size_t arraySize, const bool flipUVs, const bool flipWindingOrder) {
	Assimp::Importer importer;
	const auto scene = importer.ReadFileFromMemory(uint8_tArray, arraySize, AssimpImportFlags(flipUVs, flipWindingOrder));
	if (scene == nullptr) {
		Utils::PlatformTextOut("Failed to find model from uint8_t array, Assimp returned error: %s", importer.GetErrorString());
		return {};
	}
	return ImportAnimatedModelAssimp(scene);
}