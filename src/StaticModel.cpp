
#include "StaticModel.hpp"
#include "AnimationUtils.hpp"
#include <Assimp/scene.h>
#include "Util.hpp"
#include <assimp/Importer.hpp>


StaticMesh ImportStaticMesh(const aiScene* scene) {
	StaticMesh mesh{};
	if (scene == nullptr)
		return mesh;
	if (scene->HasMeshes()) {
		for (size_t mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
			const auto loaded_mesh = scene->mMeshes[mesh_index];
			ImportMesh(mesh, loaded_mesh, scene);
			//Get Matrix Transform from parents.
			//In GLModel Types, we don't care about this the same way as we do for the non base Mesh types.
			//For StaticMeshes, we only care about the base transform from root
			aiNode* node = scene->mRootNode->FindNode(loaded_mesh->mName);
			XMMATRIX node_transform = XMMatrixIdentity();
			while (node != nullptr) {
				XMFLOAT4X4 parent_transform_mat{};
				for (uint32_t row = 0; row < 4; row++)
				{
					const float* row_vec = node->mTransformation[row];
					for (uint32_t col = 0; col < 4; col++) {
						parent_transform_mat.m[row][col] = row_vec[col];
					}
				}
				const XMMATRIX parent_transform = XMLoadFloat4x4(&parent_transform_mat);
				node_transform *= parent_transform;
				node = node->mParent;
			}
			XMStoreFloat4x4(&mesh.rootTransform.transform,node_transform);
		}
	}
	if (scene->HasMaterials()) {
		for (uint32_t material_index = 0; material_index < scene->mNumMaterials; material_index++)
		{
			aiMaterial* material = scene->mMaterials[material_index];
			PrintMaterial(material);
		}
		aiMaterial* material = scene->mMaterials[mesh.materialIndex];
		material->GetName();
		auto count = material->GetTextureCount(aiTextureType_DIFFUSE);
		for (uint32_t tex_index = 0; tex_index < count; ++tex_index) {
			aiString path;
			uint32_t uv_index;
			aiTextureMapping mapping;
			aiTextureMapMode map_modes[3];
			material->GetTexture(aiTextureType_DIFFUSE,tex_index,&path,&mapping,&uv_index,nullptr,nullptr,map_modes);
		}
	}
	if (scene->HasTextures()) {
		for (uint32_t texture_index = 0; texture_index < scene->mNumTextures; texture_index++)
		{
			const aiTexture* texture = scene->mTextures[texture_index];
			Utils::PlatformTextOut("\nTexture Filename: %s\n\tWidth:%d\n\tHeight:%d", texture->mFilename.C_Str(),
			texture->mWidth,
			texture->mHeight);
		}
	}
	return mesh;
}

StaticMesh StaticMesh::ImportModel(const char* fileName, bool flipUVs, bool flipWindingOrder) {
	StaticMesh mesh{};
	Assimp::Importer importer;
	auto scene = importer.ReadFile(fileName, AssimpImportFlags(flipUVs, flipWindingOrder));
	return ImportStaticMesh(scene);
}

StaticMesh StaticMesh::ImportModel(uint8_t* uint8_tArray, size_t arraySize, bool flipUVs, bool flipWindingOrder) {
	Assimp::Importer importer;
	const auto scene = importer.ReadFileFromMemory(uint8_tArray, arraySize, AssimpImportFlags(flipUVs, flipWindingOrder));
	return ImportStaticMesh(scene);
}

SceneNode* AddNode(aiNode* node, SceneNode* parent, StaticModel* model) {
	if (node == nullptr ) return nullptr;
	SceneNode* scene_node = new SceneNode();
	scene_node->name = node->mName.C_Str();
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
	for (size_t childNodes = 0; childNodes < node->mNumChildren; childNodes++)
	{
		scene_node->childList.emplace_back(AddNode(node->mChildren[childNodes], scene_node, model));
	}
	return scene_node;
}

StaticModel StaticModel::ImportModel(const char* fileName, bool flipUVs, bool flipWindingOrder) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(fileName, AssimpImportFlags(flipUVs, flipWindingOrder));
	auto model = StaticModel();
	if (scene->HasMeshes()) {
		model.meshes.resize(scene->mNumMeshes);
		for (size_t mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
			const auto loaded_mesh = scene->mMeshes[mesh_index];
			Mesh mesh;
			ImportMesh(mesh, loaded_mesh, scene);
			model.meshes[mesh_index] = mesh;
		}
	}
	AddNode(scene->mRootNode,&model.rootNode,&model);
	if (scene->HasMaterials()) {
		for (size_t material_index = 0; material_index < scene->mNumMaterials; material_index++)
		{
			//TODO(Dragoon): Materials?
			aiMaterial* material = scene->mMaterials[material_index];
			PrintMaterial(material);
		}
	}
	if (scene->HasTextures()) {
		for (uint32_t texture_index = 0; texture_index < scene->mNumTextures; texture_index++)
		{
			const aiTexture* texture = scene->mTextures[texture_index];
			Utils::PlatformTextOut("\nTexture Filename: %s\n\tWidth:%d\n\tHeight:%d",texture->mFilename.C_Str(),
			texture->mWidth,
			texture->mHeight);
		}
	}
	return model;
}

StaticModel StaticModel::ImportModel(uint8_t* uint8_tArray, size_t arraySize, bool flipUVs, bool flipWindingOrder) {
	Assimp::Importer importer;
	const auto scene = importer.ReadFileFromMemory(uint8_tArray, arraySize, AssimpImportFlags(flipUVs, flipWindingOrder));
	if (scene == nullptr)
		return {};
	auto model = StaticModel();
	if (scene->HasMeshes()) {
		model.meshes.resize(scene->mNumMeshes);
		for (size_t mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
			const auto loaded_mesh = scene->mMeshes[mesh_index];
			Mesh mesh;
			ImportMesh(mesh, loaded_mesh, scene);
			model.meshes[mesh_index] = mesh;
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
	model.rootNode.name = scene->mRootNode->mName.C_Str();
	return model;
}

StaticMesh StaticMesh::ImportModel_FBX(const char* /*fileName*/, bool /*flipUVs*/, bool /*flipWindingOrder*/) {
	StaticMesh model{};
	return model;
}

StaticMesh StaticMesh::ImportModel_FBX(uint8_t* /*uint8_tArray*/, size_t /*arraySize*/, bool /*flipUVs*/, bool /*flipWindingOrder*/) {
	StaticMesh model{};
	return model;
}

StaticModel StaticModel::ImportModel_FBX(const char* /*fileName*/, bool /*flipUVs*/, bool /*flipWindingOrder*/) {
	StaticModel model{};
	return model;
}

StaticModel StaticModel::ImportModel_FBX(uint8_t* /*uint8_tArray*/, size_t /*arraySize*/, bool /*flipUVs*/, bool /*flipWindingOrder*/) {
	StaticModel model{};
	return model;
}