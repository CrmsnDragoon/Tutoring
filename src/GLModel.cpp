#include "GLModel.h"
#ifdef _WIN32
//Stop error caused by Assimp using std::min
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <iostream>
#include <cassert>
#include "Util.hpp"
#include <DirectXMath.h>

GLModel::GLModel(const std::string& GLModelPath ) {
	LoadModel(GLModelPath);
}

bool GLModel::ImportMesh(const aiMesh* mesh) {
	uint32_t numVerts = 0;
	if (mesh->HasPositions()) {
		numVerts = mesh->mNumVertices;
		indexes.clear();
		indexes.resize(mesh->mNumFaces*3);
		for (unsigned face_index = 0, total_index = 0; face_index < mesh->mNumFaces; ++face_index) {
			for (unsigned long long index = 0; index < mesh->mFaces[face_index].mNumIndices; ++index, ++total_index) {
				indexes[total_index] = mesh->mFaces[face_index].mIndices[index];
			}
		}
		positions.clear();
		positions.resize(numVerts * 3);
		for (unsigned pos_index = 0, component_count = 0; pos_index < numVerts; ++pos_index) {
			const auto pos = mesh->mVertices[pos_index];
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
			const auto normal = mesh->mNormals[normal_index];
			normals[component_count++] = normal.x;
			normals[component_count++] = normal.y;
			normals[component_count++] = normal.z;
		}
	}
	if (mesh->HasTangentsAndBitangents()) {
		tangents.clear();
		tangents.resize(numVerts*3);
		for (unsigned tangent_index = 0, component_count = 0; tangent_index < numVerts; ++tangent_index) {
			const auto tangent = mesh->mBitangents[tangent_index];
			tangents[component_count++] = tangent.x;
			tangents[component_count++] = tangent.y;
			tangents[component_count++] = tangent.z;
		}
		bitangents.clear();
		bitangents.resize(numVerts*3);
		for (unsigned binormal_index = 0, component_count = 0; binormal_index < numVerts; ++binormal_index) {
			const auto binormal = mesh->mBitangents[binormal_index];
			bitangents[component_count++] = binormal.x;
			bitangents[component_count++] = binormal.y;
			bitangents[component_count++] = binormal.z;
		}
	}
	{
		const unsigned set_index = 0;
		if (mesh->HasVertexColors(set_index)) {
			colours.clear();
			colours.resize(numVerts*4);
			for (unsigned col_index = 0, component_count = 0; col_index < numVerts; ++col_index) {
				const auto color = mesh->mColors[set_index][col_index];
				colours[component_count++] = color.r;
				colours[component_count++] = color.g;
				colours[component_count++] = color.b;
				colours[component_count++] = color.a;
			}
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
		}
	}
	const unsigned numberActiveChannels = mesh->GetNumUVChannels();
	if (numberActiveChannels > 0)
	{
		const uint32_t tex_index = 0;
		texCoords.clear();
		texCoords.resize(numVerts*2);
		for (unsigned tex_coord_index = 0, component_count = 0; tex_coord_index < numVerts; ++tex_coord_index) {
			const auto tex_coord = mesh->mTextureCoords[tex_index][tex_coord_index];
			texCoords[component_count++] = tex_coord.x;
			texCoords[component_count++] = tex_coord.y;
		}
	}
	return true;
}

bool GLModel::ImportStaticModel(const aiScene* scene) {
	if (scene == nullptr)
		return false;
	if (scene->HasMeshes()) {
		const auto loaded_mesh = scene->mMeshes[0];
		ImportMesh(loaded_mesh);
		//Simple example, just import one mesh with no base transform.
		return true;
	}
	std::cout << "Failed to find mesh in GLModel" << std::endl;
	return false;
}


bool GLModel::LoadModel(const std::string& pathToGLModel) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(pathToGLModel, Utils::AssimpImportFlags(false, false));
	if (scene == nullptr) {
		std::cout << "Failed to import mesh" << std::endl;
		return false;
	}
	return ImportStaticModel(scene);
}

void GLModel::SetupGL_1_0() {}

void GLModel::DrawGL_1_0() {
	glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, 0, positions.data());
	glColorPointer(4, GL_FLOAT, 0, colours.data());
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords.data());

	//Perform the draw
	glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, indexes.data());

    /* Cleanup states */
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

void GLModel::SetupGL_3_2() {

	assert(!positions.empty());

	std::vector<float> vertices = std::vector<float>(positions.size() + colours.size() + texCoords.size());

	unsigned cocanonated_index = 0;
	for (unsigned index = 0; index < positions.size(); ++index) {
		vertices[cocanonated_index++] = positions[index];
	}
	for (unsigned index = 0; index < colours.size(); ++index) {
		vertices[cocanonated_index++] = colours[index];
	}
	for (unsigned index = 0; index < texCoords.size(); ++index) {
		vertices[cocanonated_index++] = texCoords[index];
	}

	glGenVertexArrays(1, &vertexArrayBuffer);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindVertexArray(vertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(unsigned int),
				 indexes.data(), GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(positions.size() * sizeof(float))
	);
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void*)((positions.size() + colours.size()) * sizeof(float))
	);

	glBindVertexArray(0);
}

void GLModel::DrawGL_3_2() {
	if (!vertexArrayBuffer != 0 && vertexBuffer != 0 && indexBuffer != 0) {
		glBindVertexArray(vertexArrayBuffer);
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

bool GLModel::IsLoaded() {
	// We currently only care about the ones listed in the return statement, though it would be nice to have all 6 things.
	// && !tangents.empty() && !bitangents.empty()
	return !positions.empty() && !normals.empty() && !texCoords.empty() && !colours.empty();
}
