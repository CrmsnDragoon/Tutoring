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
		indexes.resize(numVerts);
		for (unsigned face_index = 0, total_index = 0; face_index < mesh->mNumFaces; ++face_index) {
			for (unsigned long long index = 0; index < mesh->mFaces[face_index].mNumIndices; ++index, ++total_index) {
				indexes[total_index] = mesh->mFaces[face_index].mIndices[index];
			}
		}
		positions.clear();
		positions.resize(numVerts * 3);
		for (unsigned pos_index = 0, component_count = 0; pos_index < numVerts; ++pos_index) {
			const auto pos = mesh->mVertices[indexes[pos_index]];
			positions[component_count++] = pos.x;
			positions[component_count++] = pos.y;
			positions[component_count++] = pos.z;
		}
	}
	else
		return false;
	if (mesh->HasNormals()) {
		normals.clear();
		normals.resize(numVerts*3);
		for (unsigned normal_index = 0, component_count = 0; normal_index < numVerts; ++normal_index) {
			const auto normal = mesh->mNormals[indexes[normal_index]];
			normals[component_count++] = normal.x;
			normals[component_count++] = normal.y;
			normals[component_count++] = normal.z;
		}
	}
	if (mesh->HasTangentsAndBitangents()) {
		tangents.clear();
		tangents.resize(numVerts*3);
		for (unsigned tangent_index = 0, component_count = 0; tangent_index < numVerts; ++tangent_index) {
			const auto tangent = mesh->mBitangents[indexes[tangent_index]];
			tangents[component_count++] = tangent.x;
			tangents[component_count++] = tangent.y;
			tangents[component_count++] = tangent.z;
		}
		binormals.clear();
		binormals.resize(numVerts*3);
		for (unsigned binormal_index = 0, component_count = 0; binormal_index < numVerts; ++binormal_index) {
			const auto binormal = mesh->mBitangents[indexes[binormal_index]];
			binormals[component_count++] = binormal.x;
			binormals[component_count++] = binormal.y;
			binormals[component_count++] = binormal.z;
		}
	}
	for (unsigned set_index = 0; set_index < AI_MAX_NUMBER_OF_COLOR_SETS; set_index++)
	{
		if (mesh->HasVertexColors(set_index)) {
			colours.clear();
			colours.resize(numVerts*4);
			for (unsigned col_index = 0, component_count = 0; col_index < numVerts; ++col_index) {
				const auto color = mesh->mColors[set_index][indexes[col_index]];
				colours[component_count++] = color.r;
				colours[component_count++] = color.g;
				colours[component_count++] = color.b;
				colours[component_count++] = color.a;
			}
			break;
		}
		else {
			colours.clear();
			colours.resize(numVerts*4);
			for (unsigned col_index = 0, component_count = 0; col_index < numVerts; ++col_index) {
				colours[component_count++] = 1;
				colours[component_count++] = 1;
				colours[component_count++] = 1;
				colours[component_count++] = 1;
			}
			break;
		}
	}
	unsigned numberActiveChannels = mesh->GetNumUVChannels();
	for (unsigned tex_index = 0; tex_index < numberActiveChannels; tex_index++)
	{
		texCoords.clear();
		texCoords.resize(numVerts*2);
		for (unsigned tex_coord_index = 0, component_count = 0; tex_coord_index < numVerts; ++tex_coord_index) {
			const auto tex_coord = mesh->mTextureCoords[tex_index][indexes[tex_coord_index]];
			texCoords[component_count++] = tex_coord.x;
			texCoords[component_count++] = tex_coord.y;
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
	auto scene = importer.ReadFile(pathToModel, AssimpImportFlags(true, false));
	if (scene == nullptr) {
		std::cout << "Failed to import mesh" << std::endl;
		return false;
	}
	return ImportStaticMesh(scene);
}

void Model::DrawGL_1_0() {
	glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
    glVertexPointer(3, GL_FLOAT, 0, &positions[0]);
    glColorPointer(4, GL_FLOAT, 0, &colours[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);

	//Perform the draw
    glDrawArrays(GL_TRIANGLES, 0, indexes.size());

    /* Cleanup states */
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

bool Model::IsLoaded() {
	// We currently only care about the ones listed in the return statement, though it would be nice to have all 6 things.
	// && !tangents.empty() && !binormals.empty()
	return !positions.empty() && !normals.empty() && !texCoords.empty() && !colours.empty() ;
}
