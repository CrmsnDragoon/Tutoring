#include "Model.h"
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model(const std::string& modelPath ) {
	LoadModel(modelPath);
}

bool Model::ImportMesh(const aiMesh* mesh) {
	uint32_t numVerts = 0;
	if (mesh->HasPositions()) {
		numVerts = 3ull * mesh->mNumFaces;
		indexes.clear();
		indexes.resize(3ull * mesh->mNumFaces);
		for (unsigned face_index = 0, total_index = 0; face_index < mesh->mNumFaces; ++face_index) {
			for (unsigned long long index = 0; index < mesh->mFaces[face_index].mNumIndices; ++index, ++total_index) {
				indexes[total_index] = mesh->mFaces[face_index].mIndices[index];
			}
		}
		positions.clear();
		positions.resize(numVerts*3);
		for (unsigned face_index = 0, componentCount = 0; face_index < mesh->mNumFaces; ++face_index) {
			for (unsigned long long index = 0, total_index = 0; index < mesh->mFaces[face_index].mNumIndices; ++index, ++total_index) {
				const auto pos = mesh->mVertices[mesh->mFaces[face_index].mIndices[index]];
				positions[componentCount++] = pos.x;
				positions[componentCount++] = pos.y;
				positions[componentCount++] = pos.z;
			}
		}
	}
	else
		return false;
	for (unsigned set_index = 0; set_index < AI_MAX_NUMBER_OF_COLOR_SETS; set_index++)
	{
		if (mesh->HasVertexColors(set_index)) {
			colours.clear();
			colours.resize(numVerts*4);
			for (unsigned col_index = 0, componentCount = 0; col_index < numVerts; ++col_index) {
				const auto color = mesh->mColors[set_index][indexes[col_index]];
				colours[componentCount++] = color.r;
				colours[componentCount++] = color.g;
				colours[componentCount++] = color.b;
				colours[componentCount++] = color.a;
			}
		}
		else {
			colours.clear();
			colours.resize(numVerts*4);
			for (unsigned col_index = 0, componentCount = 0; col_index < numVerts; ++col_index) {
				colours[componentCount++] = 1;
				colours[componentCount++] = 1;
				colours[componentCount++] = 1;
				colours[componentCount++] = 1;
			}
		}
		break;
	}
	for (unsigned tex_index = 0; tex_index < mesh->GetNumUVChannels(); tex_index++)
	{
		texCoords.clear();
		texCoords.resize(numVerts*2);
		for (unsigned tex_coord_index = 0, componentCount = 0; tex_coord_index < numVerts; ++tex_coord_index) {
			const auto tex_coord = mesh->mTextureCoords[tex_index][indexes[tex_coord_index]];
			texCoords[componentCount++] = tex_coord.x;
			texCoords[componentCount++] = tex_coord.y;
		}
		break;
	}
	return true;
}

bool Model::ImportStaticMesh(const aiScene* scene) {
	if (scene == nullptr)
		return false;
	if (scene->HasMeshes()) {
		for (unsigned long long mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
			const auto loaded_mesh = scene->mMeshes[mesh_index];
			ImportMesh(loaded_mesh);
			//Simple example, just import one mesh with no base transform.
			return true;
		}
	}
	std::cout << "Failed to find mesh in model" << std::endl;
	return false;
}

uint32_t AssimpImportFlags(const bool flipUVs, const bool flipWindingOrder) {
	uint32_t flags = aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenNormals;
	if (flipUVs) { flags |= aiProcess_FlipUVs; }
	if (flipWindingOrder) { flags |= aiProcess_FlipWindingOrder; }
	return flags;
}

bool Model::LoadModel(const std::string& pathToModel) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(pathToModel, AssimpImportFlags(false, false));
	if (scene == nullptr) {
		std::cout << "Failed to import mesh" << std::endl;
		return false;
	}
	return ImportStaticMesh(scene);
}

void Model::DrawGL_1_0() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &positions[0]);
    //glIndexPointer(GL_UNSIGNED_INT, 0, &indexes[0]);
    glColorPointer(4, GL_FLOAT, 0, &colours[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);

    /* Send data : 24 vertices */
    glDrawArrays(GL_TRIANGLES, 0, indexes.size());

    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

bool Model::IsLoaded() {
	return !positions.empty() && !colours.empty() && !texCoords.empty();
}
