#pragma once
#include "Model.hpp"
#include "AnimatedMesh.hpp"
#include "AnimationClip.hpp"
#include <glad/glad.h>

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
		
		struct Vertex {
		    XMFLOAT3 Position;
		    XMFLOAT3 Normal;
		    XMFLOAT2 TexCoords;
		    XMUINT4 BoneIndices;
		    XMFLOAT4 BoneWeights;
		};

		for (const auto& mesh : meshes) {
			std::vector<unsigned int> indices = mesh.indices;
			std::vector<Vertex> vertices = std::vector<Vertex>(mesh.positions.size());

			for (unsigned index = 0; index < mesh.positions.size(); ++index) {
				vertices[index] = {
					mesh.positions[index],
					mesh.normals[index],
					{mesh.texCoords[0][index].x, mesh.texCoords[0][index].y},
					mesh.boneWeights[index].boneIndexes,
					mesh.boneWeights[index].boneWeights
				};
			}
		
			GLuint vertexArrayBuffer;
			GLuint vertexBuffer;
			GLuint indexBuffer;

			glGenVertexArrays(1, &vertexArrayBuffer);
			glGenBuffers(1, &vertexBuffer);
			glGenBuffers(1, &indexBuffer);
	  
			glBindVertexArray(vertexArrayBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
			             &indices[0], GL_STATIC_DRAW);

		    // vertex positions
		    glEnableVertexAttribArray(0);	
		    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		    // vertex normals
		    glEnableVertexAttribArray(1);	
		    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		    // vertex texture coords
		    glEnableVertexAttribArray(2);	
		    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		    glEnableVertexAttribArray(3);	
		    glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneIndices));
		    glEnableVertexAttribArray(4);	
		    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneWeights));

		    glBindVertexArray(0);

			mesh.vertexArrayBuffer = vertexArrayBuffer;
			mesh.vertexBuffer = vertexBuffer;
			mesh.indexBuffer = indexBuffer;
		}
		
	}
};

