#version 330 compatibility

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 vecColour;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in uvec4 boneIndexes;
layout (location = 5) in vec4 boneWeights;

out vec4 colour;
out vec2 texCoord;

layout (binding = 0, column_major) uniform BoneBuffer {
	mat4 boneMats[256];
} boneBuffer;

void main()
{
	mat4 skinTransform;
	skinTransform += boneBuffer.boneMats[boneIndexes.x] * boneWeights.x;
	skinTransform += boneBuffer.boneMats[boneIndexes.y] * boneWeights.y;
	skinTransform += boneBuffer.boneMats[boneIndexes.z] * boneWeights.z;
	skinTransform += boneBuffer.boneMats[boneIndexes.w] * boneWeights.w;
	
	vec4 position = mul(skinTransform,vec4(pos.xyz , 1.0));

	gl_Position = mul(gl_ProjectionMatrix*gl_ModelViewMatrix,position);
	colour = vecColour;
	texCoord = texcoord;
}