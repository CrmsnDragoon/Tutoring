#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <assimp/material.h>
using namespace DirectX;


	enum class MaterialShading {
		flat_shading,
		gourand_shading,
		phong_shading,
		blinn_phong_shading,
		toon_shading,
		//Lambertian with roughness
		oren_nayar_shading,
		//Lambertian with darkness
		minnaert_shading,
		//metallic
		cook_torrance_shading,
		no_shading,
		fresnel_shading,
	};
struct Material {
	XMFLOAT3 diffuse{1,1,1};
	float opacity = 1;
	XMFLOAT3 ambient{0,0,0};
	XMFLOAT3 specular{0,0,0};
	XMFLOAT3 emissive{0,0,0};
	float shininess = 1;
	float shininessStrength = 1;
		MaterialShading shadingType = MaterialShading::blinn_phong_shading;
	std::vector<std::string> texturePaths;
		void LoadMaterial(aiMaterial*);
};
