#version 330 core

in vec4 colour;
in vec2 texCoord;

out vec4 FragColour;

uniform sampler2D tex;

void main()
{
	FragColour = texture(tex,texCoord) * colour;
}