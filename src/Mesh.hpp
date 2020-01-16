#pragma once
#include "DirectXMath.h"
#include "SceneNode.hpp"
#include "Material.hpp"
#include <glad/glad.h>

class Mesh {
	friend class Model;
private:
protected:
	unsigned vertexArrayBuffer;
	unsigned vertexBuffer;
	unsigned indexBuffer;
public:
	std::string name;
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT3> tangents;
	// ReSharper disable once IdentifierTypo
	std::vector<XMFLOAT3> binormals;
	std::vector<XMFLOAT3> texCoords[8];
	uint32_t numberActiveTexCoordChannels = 0;
	std::vector<XMFLOAT4> vertexColours[8];
	std::vector<uint32_t> indices;
	SceneNode* sceneNode = nullptr;
	uint32_t materialIndex;
	Material material;
	void virtual setupGL();
	void virtual Draw3_2() const;
	void Draw1_0();
};

inline void Mesh::setupGL() {

	assert(!positions.empty());

	struct Vertex {
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT4 Colour;
		XMFLOAT2 TexCoords;
	};
	std::vector<Vertex> vertices = std::vector<Vertex>(positions.size());

	for (unsigned index = 0; index < positions.size(); ++index) {
		vertices[index] = {
			positions[index],
			normals[index],
			vertexColours[0][index],
			{texCoords[0][index].x, texCoords[0][index].y}
		};
	}

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
	// vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Colour));
	// vertex texture coords
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

inline void Mesh::Draw3_2() const {
	// draw mesh
	glBindVertexArray(vertexArrayBuffer);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	/*glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
	
    glActiveTexture(GL_TEXTURE0);*/
}

inline void Mesh::Draw1_0() {
	glPushMatrix();

	//glLoadMatrixf(&sceneNode->Transform().m[0][0]);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, positions.data());
	glNormalPointer(GL_FLOAT, 0, normals.data());
	glColorPointer(4, GL_FLOAT, 0, vertexColours[0].data());
	glTexCoordPointer(3, GL_FLOAT, 0, texCoords->data());

	//Perform the draw
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	/* Cleanup states */
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

}
