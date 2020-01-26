#include "Material.hpp"
#include "AnimationUtils.hpp"
#include "Util.hpp"


void Material::LoadMaterial(aiMaterial* material) {
	aiString name;
	auto res = material->Get(AI_MATKEY_NAME, name);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tMaterial: %s", name.C_Str());
	int shading_model;
	res = material->Get(AI_MATKEY_SHADING_MODEL, shading_model);
	if (res == AI_SUCCESS) {
		std::string shading_mode;
		switch (shading_model) {
		case aiShadingMode::aiShadingMode_Flat:
			shadingType = MaterialShading::flat_shading;
			break;
		case aiShadingMode::aiShadingMode_Gouraud:
			shadingType = MaterialShading::gourand_shading;
			break;
		case aiShadingMode::aiShadingMode_Phong:
			shadingType = MaterialShading::phong_shading;
			break;
		case aiShadingMode::aiShadingMode_Blinn:
			shadingType = MaterialShading::blinn_phong_shading;
			break;
		case aiShadingMode::aiShadingMode_Toon:
			shadingType = MaterialShading::toon_shading;
			break;
		case aiShadingMode::aiShadingMode_OrenNayar:
			shadingType = MaterialShading::oren_nayar_shading;
			break;
		case aiShadingMode::aiShadingMode_Minnaert:
			shadingType = MaterialShading::minnaert_shading;
			break;
		case aiShadingMode::aiShadingMode_CookTorrance:
			shadingType = MaterialShading::cook_torrance_shading;
			break;
		case aiShadingMode::aiShadingMode_NoShading:
			shadingType = MaterialShading::no_shading;
			break;
		case aiShadingMode::aiShadingMode_Fresnel:
			shadingType = MaterialShading::fresnel_shading;
			break;
		default:
			//Defaults to Blinn-phong
			break;
		}
	}
	
	aiColor3D colour (0.f,0.f,0.f);
	res = material->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
	if (res == AI_SUCCESS)
		diffuse = {colour.r, colour.g, colour.b};
	res = material->Get(AI_MATKEY_COLOR_SPECULAR, colour);
	if (res == AI_SUCCESS)
		specular = {colour.r, colour.g, colour.b};
	res = material->Get(AI_MATKEY_COLOR_AMBIENT, colour);
	if (res == AI_SUCCESS)
		ambient = {colour.r, colour.g, colour.b};
	res = material->Get(AI_MATKEY_COLOR_EMISSIVE, colour);
	if (res == AI_SUCCESS)
		emissive = {colour.r, colour.g, colour.b};

	f32 mtl_opacity, mtl_shininess, shininess_strength;
	res = material->Get(AI_MATKEY_OPACITY, mtl_opacity);
	if (res == AI_SUCCESS)
		this->opacity = mtl_opacity;
	res = material->Get(AI_MATKEY_SHININESS, mtl_shininess);
	if (res == AI_SUCCESS)
		this->shininess = mtl_shininess;
	res = material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);
	if (res == AI_SUCCESS)
		this->shininessStrength = shininess_strength;

	const auto count_for_none_type = material->GetTextureCount(aiTextureType_NONE);
	if (count_for_none_type > 0)
		Utils::PlatformTextOut("\n\tCount type None Parameters: %d", count_for_none_type);
	
	//aiTextureType_NONE is 0, we'll handle that separately.
	for(aiTextureType type = aiTextureType_DIFFUSE; type < AI_TEXTURE_TYPE_MAX; type = aiTextureType(int(type) + 1))
	{
		const auto count_for_current_type = material->GetTextureCount(type);
		if (count_for_current_type > 0)
			Utils::PlatformTextOut("\n\tType: %s", TextureTypeToString(type).c_str());
				
		for (u32 i = 0; i < count_for_current_type; i++)
		{
			aiString path;
			aiTextureMapping mapping;
			u32 uv_index;
			f32 blend;
			aiTextureOp op;
			aiTextureMapMode mapMode;
			const auto return_val = material->GetTexture(type, i, &path, &mapping, &uv_index, &blend, &op, &mapMode);
			if (return_val == aiReturn_SUCCESS){
				Utils::PlatformTextOut("\n\tTexture: %i\n\tPath:%s", i, path.C_Str());
				Utils::PlatformTextOut("\n\t\tUV Index: %i", uv_index);
				Utils::PlatformTextOut("\n\t\tBlend: %f.03", blend);
				Utils::PlatformTextOut("\n\t\tTexture Op: %s", OpToStr(op).c_str());
				Utils::PlatformTextOut("\n\t\tTexture Map Mode: %s", MapModeToStr(mapMode).c_str());
			}
		}
	}
}
