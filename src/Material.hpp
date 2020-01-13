#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

using namespace DirectX;

struct Material {
	XMFLOAT3 diffuse{1,1,1};
	float opacity = 1;
	XMFLOAT3 ambient{0,0,0};
	XMFLOAT3 specular{0,0,0};
	XMFLOAT3 emissive{0,0,0};
	float shininess = 1;
	float shininessStrength = 1;
	std::vector<std::string> texturePaths;
};
