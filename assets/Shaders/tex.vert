#version 330 compatibility

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 vecColour;
layout (location = 3) in vec2 texcoord;
layout (location = 4) in uvec4 boneIndexes;
layout (location = 5) in vec4 boneWeights;

out vec4 colour;
out vec2 texCoord;

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos.xyz,1.0);
	colour = vecColour;
	texCoord = texcoord;
}