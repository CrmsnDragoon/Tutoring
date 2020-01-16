
#include "AnimatedMesh.hpp"

void AnimatedMesh::setupGL() {

	assert(!positions.empty());
	assert(!normals.empty());
	assert(!texCoords[0].empty());

	if (boneWeights.empty()) {
		this->Mesh::setupGL();
		return;
	}
	
	struct Vertex {
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoords;
		XMUINT4 BoneIndices;
		XMFLOAT4 BoneWeights;
	};
	std::vector<Vertex> vertices = std::vector<Vertex>(positions.size());

	for (unsigned index = 0; index < positions.size(); ++index) {
		vertices[index] = {
			positions[index],
			normals[index],
			{texCoords[0][index].x, texCoords[0][index].y},
			boneWeights[index].boneIndexes,
			boneWeights[index].boneWeights
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
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneIndices));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneWeights));

	glBindVertexArray(0);
}

void AnimatedMesh::Draw3_2() const {
	// draw mesh
	glBindVertexArray(vertexArrayBuffer);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	/*glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
	
    glActiveTexture(GL_TEXTURE0);*/
}