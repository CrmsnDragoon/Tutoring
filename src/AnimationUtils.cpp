#include "AnimationUtils.hpp"
#ifdef _WIN32
//Stop error caused by Assimp using std::min
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <DirectXMath.h>
#include "Util.hpp"


using namespace DirectX;

#pragma region printFuncs

std::string OpToStr(aiTextureOp op) {
	std::string str;
	switch (op) {
	case aiTextureOp_Multiply:
		str = "Multiply";
		break;
	case aiTextureOp_Add:
		str = "Add";
		break;
	case aiTextureOp_Subtract: 
		str = "Subtract";
		break;
	case aiTextureOp_Divide: 
		str = "Divide?!";
		break;
	case aiTextureOp_SmoothAdd: 
		str = "SmoothAdd";
		break;
	case aiTextureOp_SignedAdd: 
		str = "SignedAdd";
		break;
	case _aiTextureOp_Force32Bit: 
		str = "Force32Bit";
		break;
	default: ;
	}
	return str;
}

std::string MapModeToStr(aiTextureMapMode op) {
	std::string str;
	switch (op) {
	case aiTextureMapMode_Wrap: 
		str = "Wrap";
		break;
	case aiTextureMapMode_Clamp: 
		str = "Clamp";
		break;
	case aiTextureMapMode_Decal: 
		str = "Decal";
		break;
	case aiTextureMapMode_Mirror: 
		str = "Mirror";
		break;
	case _aiTextureMapMode_Force32Bit: 
		str = "Force32Bit";
		break;
	default: ;
	}
	return str;
}

std::string TextureTypeToString(aiTextureType type) {
	switch (type) {
		case aiTextureType_NONE: return std::string("NONE");
		case aiTextureType_DIFFUSE: return std::string("DIFFUSE");
		case aiTextureType_SPECULAR: return std::string("SPECULAR");
		case aiTextureType_AMBIENT: return std::string("AMBIENT");
		case aiTextureType_EMISSIVE: return std::string("EMISSIVE");
		case aiTextureType_HEIGHT: return std::string("HEIGHT");
		case aiTextureType_NORMALS: return std::string("NORMALS");
		case aiTextureType_SHININESS: return std::string("SHININESS");
		case aiTextureType_OPACITY: return std::string("OPACITY");
		case aiTextureType_DISPLACEMENT: return std::string("DISPLACEMENT");
		case aiTextureType_LIGHTMAP: return std::string("LIGHTMAP");
		case aiTextureType_REFLECTION: return std::string("REFLECTION");
		case aiTextureType_BASE_COLOR: return std::string("BASE_COLOUR");
		case aiTextureType_NORMAL_CAMERA: return std::string("NORMAL_CAMERA");
		case aiTextureType_EMISSION_COLOR: return std::string("EMISSION_COLOUR");
		case aiTextureType_METALNESS: return std::string("METALNESS");
		case aiTextureType_DIFFUSE_ROUGHNESS: return std::string("DIFFUSE_ROUGHNESS");
		case aiTextureType_AMBIENT_OCCLUSION: return std::string("AMBIENT_OCCLUSION");
		case aiTextureType_UNKNOWN: return std::string("UNKNOWN");
		case _aiTextureType_Force32Bit: return std::string("Force32Bit");
		default: return std::string();
	}
}

void PrintMaterial(aiMaterial* material) {
#if !CACONYM_3D_PRINT_MATERIALS || !defined(CACONYM_3D_PRINT_MATERIALS)
	UNREFERENCED_PARAMETER(material);
#elif CACONYM_3D_PRINT_MATERIALS
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
			shading_mode = "Flat, faceted shading";
			break;
		case aiShadingMode::aiShadingMode_Gouraud:
			shading_mode = "Gourand";
			break;
		case aiShadingMode::aiShadingMode_Phong:
			shading_mode = "Phong";
			break;
		case aiShadingMode::aiShadingMode_Blinn:
			shading_mode = "Blinn-Phong";
			break;
		case aiShadingMode::aiShadingMode_Toon:
			shading_mode = "Toon";
			break;
		case aiShadingMode::aiShadingMode_OrenNayar:
			shading_mode = "Oren Nayar";
			break;
		case aiShadingMode::aiShadingMode_Minnaert:
			shading_mode = "Minnaert";
			break;
		case aiShadingMode::aiShadingMode_CookTorrance:
			shading_mode = "Cook Torrance";
			break;
		case aiShadingMode::aiShadingMode_NoShading:
			shading_mode = "NoShading";
			break;
		case aiShadingMode::aiShadingMode_Fresnel:
			shading_mode = "Fresnel";
			break;
		default:
			shading_mode = "Unknown value";
		}
		
		Utils::PlatformTextOut("\n\tShading Model: %s", shading_mode.c_str());
	}
	
	aiColor3D colour (0.f,0.f,0.f);
	res = material->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tDiffuse Colour: %f %f %f", colour.r, colour.g, colour.b);
	res = material->Get(AI_MATKEY_COLOR_SPECULAR, colour);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tSpecular Colour: %f %f %f", colour.r, colour.g, colour.b);
	res = material->Get(AI_MATKEY_COLOR_AMBIENT, colour);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tAmbient Colour: %f %f %f", colour.r, colour.g, colour.b);
	res = material->Get(AI_MATKEY_COLOR_EMISSIVE, colour);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tEmissive Colour: %f %f %f", colour.r, colour.g, colour.b);
	res = material->Get(AI_MATKEY_COLOR_REFLECTIVE, colour);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tReflective Colour: %f %f %f", colour.r, colour.g, colour.b);
	res = material->Get(AI_MATKEY_COLOR_TRANSPARENT, colour);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tTransparent Colour: %f %f %f", colour.r, colour.g, colour.b);
	bool wireframe;
	res = material->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tWireframe: %s", wireframe ? "True":"False");
	bool two_sided;
	res = material->Get(AI_MATKEY_TWOSIDED, two_sided);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tTwoSided: %s", two_sided ? "True":"False");

	float opacity, shininess, shininess_strength, refractive_index, transparency_factor, reflectivity, bump_scaling;
	res = material->Get(AI_MATKEY_OPACITY, opacity);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tOpacity: %f", opacity);
	res = material->Get(AI_MATKEY_SHININESS, shininess);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tShininess: %f", shininess);
	res = material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tShininess Strength: %f", shininess_strength);
	res = material->Get(AI_MATKEY_REFRACTI, refractive_index);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tRefractive Index: %f", refractive_index);
	res = material->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparency_factor);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tTransparent factor: %f", transparency_factor);
	res = material->Get(AI_MATKEY_REFLECTIVITY, reflectivity);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tReflectivity: %f", reflectivity);
	res = material->Get(AI_MATKEY_BUMPSCALING, bump_scaling);
	if (res == AI_SUCCESS)
		Utils::PlatformTextOut("\n\tBump Scaling: %f", bump_scaling);
	{
		Utils::PlatformTextOut("\n\n\t Material Property Keys: ");
		for (size_t i = 0; i < material->mNumProperties; i++)
		{
			Utils::PlatformTextOut("%s, ",material->mProperties[i]->mKey.C_Str());
		}
		Utils::PlatformTextOut("\n");
	}
	
	const auto count_for_none_type = material->GetTextureCount(aiTextureType_NONE);
	if (count_for_none_type > 0)
		Utils::PlatformTextOut("\n\tType None Parameters: %d", count_for_none_type);
	
	//aiTextureType_NONE is 0, we'll handle that separately.
	for(aiTextureType type = aiTextureType_DIFFUSE; type < AI_TEXTURE_TYPE_MAX; type = aiTextureType(int(type) + 1))
	{
		const auto count_for_current_type = material->GetTextureCount(type);
		if (count_for_current_type > 0)
			Utils::PlatformTextOut("\n\tType: %s", TextureTypeToString(type).c_str());
				
		for (uint32_t i = 0; i < count_for_current_type; i++)
		{
			aiString path;
			aiTextureMapping mapping;
			uint32_t uv_index;
			float blend;
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
	Utils::PlatformTextOut("\n");
#endif
}

std::string AnimBehaviourToString(aiAnimBehaviour behaviour){
	switch (behaviour) {
		case aiAnimBehaviour_DEFAULT:
			return std::string("DEFAULT");
		case aiAnimBehaviour_CONSTANT:
			return std::string("CONSTANT");
		case aiAnimBehaviour_LINEAR:
			return std::string("LINEAR");
		case aiAnimBehaviour_REPEAT:
			return std::string("REPEAT");
		case _aiAnimBehaviour_Force32Bit: 
			return std::string("_aiAnimBehaviour_Force32Bit");
	}
	return std::string("");
};
void PrintSkeleton(const SceneNode* sceneNode, const int skeletonDepth) {
#if !CACONYM_3D_PRINT_SKELETON || !defined(CACONYM_3D_PRINT_SKELETON)
	UNREFERENCED_PARAMETER(sceneNode);
	UNREFERENCED_PARAMETER(skeletonDepth);
#else 
  for (int currentDepth = 0; currentDepth < skeletonDepth; ++currentDepth) {
		Utils::PlatformTextOut("\t");
	}
	Bone* bone = dynamic_cast<Bone*>(sceneNode);

	if (bone != nullptr) {
		Utils::PlatformTextOut("%d:",bone->Index());
	}
	
	if (sceneNode->parent != nullptr)
		Utils::PlatformTextOut("%s, parent: %s\n",sceneNode->name.c_str(), sceneNode->parent->name.c_str());
	else
		Utils::PlatformTextOut("%s, parent: None\n",sceneNode->name.c_str());
	
	for (auto child : sceneNode->childList) {
		PrintSkeleton(child, skeletonDepth+1);
	}
#endif

}

void PrintAnimation(const aiAnimation* anim) {
#if !CACONYM_3D_PRINT_ANIMATIONS || !defined(CACONYM_3D_PRINT_ANIMATIONS)
	UNREFERENCED_PARAMETER(anim);
#else
	Utils::PlatformTextOut("\nAnimation: %s",anim->mName.C_Str());
	Utils::PlatformTextOut("\n\tDuration: %f",anim->mDuration);
	for (size_t channel_index = 0; channel_index < anim->mNumChannels; channel_index++)
	{
		auto channel = anim->mChannels[channel_index];
		Utils::PlatformTextOut("\n\t\tChannel: %s",channel->mNodeName.C_Str());
		std::string behaviourName = AnimBehaviourToString(channel->mPreState);
		Utils::PlatformTextOut("\n\t\t\t pre: %s",behaviourName.c_str());
		for (size_t key_index = 0; key_index < 	channel->mNumPositionKeys; key_index++)
		{
			const double time = channel->mPositionKeys[key_index].mTime;
			const aiVector3D position = channel->mPositionKeys[key_index].mValue;
			Utils::PlatformTextOut("\n\t\t\t\t Position Key @ time %f: (%f, %f, %f)",
				time, position.x, position.y, position.z);
		}
		for (size_t key_index = 0; key_index < channel->mNumRotationKeys; key_index++)
		{
			const double time = channel->mRotationKeys[key_index].mTime;
			const aiQuaternion quaternoin = channel->mRotationKeys[key_index].mValue;
			Utils::PlatformTextOut("\n\t\t\t\t Rotation Key @ time %f: (%f, %f, %f, %f)",
			    time, quaternoin.x, quaternoin.y, quaternoin.z, quaternoin.w);
		}
		for (size_t key_index = 0; key_index < channel->mNumScalingKeys; key_index++)
		{
			const double time = channel->mScalingKeys[key_index].mTime;
			const aiVector3D scale = channel->mScalingKeys[key_index].mValue;
			Utils::PlatformTextOut("\n\t\t\t\t Scale Key @ time %f: (%f, %f, %f)",
				time, scale.x, scale.y, scale.z);
		}
		behaviourName = AnimBehaviourToString(channel->mPostState);
		Utils::PlatformTextOut("\n\t\t\t post: %s\n",behaviourName.c_str());
	}
	for (size_t channel_index = 0; channel_index < anim->mNumMeshChannels; channel_index++)
	{
		auto channel = anim->mMeshChannels[channel_index];
		Utils::PlatformTextOut("\n\t\tMesh Channel: %s",channel->mName.C_Str());
		for (size_t key_index = 0; key_index < 	channel->mNumKeys; key_index++)
		{
			const double time = channel->mKeys[key_index].mTime;
			Utils::PlatformTextOut("\n\t\t\t\t Mesh Key At Time %f: AnimMesh Index: %d", time,
				channel->mKeys[key_index].mValue);
		}
	}
	for (size_t channel_index = 0; channel_index < anim->mNumMorphMeshChannels; channel_index++)
	{
		auto channel = anim->mMorphMeshChannels[channel_index];
		Utils::PlatformTextOut("\n\t\tMorph Mesh Channel: %s",channel->mName.C_Str());
		for (size_t key_index = 0; key_index < 	channel->mNumKeys; key_index++)
		{
			const double time = channel->mKeys[key_index].mTime;
			Utils::PlatformTextOut("\n\t\t\t\t Morph Key At Time: %f ", time);
			for (size_t val_weight_index = 0; val_weight_index < channel->mKeys[val_weight_index].mNumValuesAndWeights; val_weight_index++)
			{
				Utils::PlatformTextOut("\n\t\t\t\t\t Morph Weight: %f, Morph Value: %f", 
				    channel->mKeys[key_index].mWeights[val_weight_index], 
					channel->mKeys[key_index].mValues[val_weight_index]);
			}
		}
	}
	Utils::PlatformTextOut("\n");
#endif
}

#pragma endregion
uint32_t AssimpImportFlags(const bool flipUVs, const bool flipWindingOrder) {
	uint32_t flags = aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_LimitBoneWeights | 
		aiProcess_FindInvalidData |
		aiProcess_CalcTangentSpace |
		aiProcessPreset_TargetRealtime_MaxQuality;

	//Convert to left handed is MakeLeftHanded and the following
	if (flipUVs) { flags |= aiProcess_FlipUVs; }
	if (flipWindingOrder) { flags |= aiProcess_FlipWindingOrder; }
	return flags;
}

void ImportMesh(Mesh& out, const aiMesh* mesh, const aiScene* scene) {
	out.name = mesh->mName.C_Str();
	if (!mesh->HasPositions()) return;
	const uint32_t numVerts = mesh->mNumVertices;
	out.positions.resize(numVerts);
	for (uint32_t pos_index = 0; pos_index < numVerts; ++pos_index) {
		const auto pos = mesh->mVertices[pos_index];
		out.positions[pos_index] = (XMFLOAT3(pos.x, pos.y, pos.z));
	}
	const uint32_t numFaces = mesh->mNumFaces;
	out.indices.resize(3ull * size_t(numFaces));
	for (uint32_t face_index = 0, total_index = 0; face_index < numFaces; ++face_index) {
		//Triangulate is on by default
		for (size_t index = 0; index < 3/*mesh->mFaces[face_index].mNumIndices*/; ++index, ++total_index) {
			out.indices[total_index] = mesh->mFaces[face_index].mIndices[index];
		}
	}
	if (mesh->HasNormals()) {
		out.normals.resize(numVerts);
		for (uint32_t normal_index = 0; normal_index < numVerts; ++normal_index) {
			const auto normal = mesh->mNormals[normal_index];
			out.normals[normal_index] = XMFLOAT3(normal.x, normal.y, normal.z);
		}
	}
	if (mesh->HasTangentsAndBitangents()) {
		out.tangents.resize(numVerts);
		out.binormals.resize(numVerts);
		for (uint32_t tangent_index = 0; tangent_index < numVerts; ++tangent_index) {
			const auto tangent = mesh->mTangents[tangent_index];
			out.tangents[tangent_index] = XMFLOAT3(tangent.x, tangent.y, tangent.z);
		}
		for (uint32_t binormal_index = 0; binormal_index < numVerts; ++binormal_index) {
			const auto binormal = mesh->mBitangents[binormal_index];
			out.binormals[binormal_index] = XMFLOAT3(binormal.x, binormal.y, binormal.z);
		}
	}
	for (uint32_t set_index = 0; set_index < AI_MAX_NUMBER_OF_COLOR_SETS; set_index++)
	{
		if (mesh->HasVertexColors(set_index)) {
			out.vertexColours[set_index].resize(numVerts);
			for (uint32_t col_index = 0; col_index < numVerts; ++col_index) {
				const auto color = mesh->mColors[set_index][col_index];
				out.vertexColours[set_index][col_index] = XMFLOAT4(color.r, color.g, color.b, color.a);
			}
			break;
		}
	}
	out.materialIndex = mesh->mMaterialIndex;

	if (out.vertexColours[0].empty()) {
		aiColor3D colour (0.f,0.f,0.f);
		auto res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, colour);
		if (res == AI_SUCCESS) {
			out.material.diffuse = XMFLOAT3(colour.r, colour.g, colour.b);
		}
		res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_COLOR_SPECULAR, colour);
		if (res == AI_SUCCESS) {
			out.material.specular = XMFLOAT3(colour.r, colour.g, colour.b);
		}
		res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_COLOR_AMBIENT, colour);
		if (res == AI_SUCCESS) {
			out.material.ambient = XMFLOAT3(colour.r, colour.g, colour.b);
		}
		res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_COLOR_EMISSIVE, colour);
		if (res == AI_SUCCESS) {
			out.material.emissive = XMFLOAT3(colour.r, colour.g, colour.b);
		}
		float opacity, shininess, shininess_strength;
		res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_OPACITY, opacity);
		if (res == AI_SUCCESS) {
			out.material.opacity = opacity; //Defaults to 1
		}
		res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_SHININESS, shininess);
		if (res == AI_SUCCESS) {
			out.material.shininess = shininess;
		}
		res = scene->mMaterials[out.materialIndex]->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);
		if (res == AI_SUCCESS) {
			out.material.shininessStrength = shininess_strength;
		}

		//Now that we have material information, just copy the diffuse value across, it's ok.
		out.vertexColours[0].resize(numVerts);
		
		auto vec = XMLoadFloat3(&out.material.diffuse);
		vec = XMVectorSetByIndex(vec, out.material.opacity,3);
		for (uint32_t colour_index = 0; colour_index < numVerts; ++colour_index) {
			XMStoreFloat4(&out.vertexColours[0][colour_index],vec);
		}
	}
	
	out.numberActiveTexCoordChannels = mesh->GetNumUVChannels();
	for (uint32_t tex_index = 0; tex_index < out.numberActiveTexCoordChannels; tex_index++)
	{
		out.texCoords[tex_index].resize(mesh->mNumVertices);
		for (uint32_t tex_coord_index = 0; tex_coord_index < numVerts; ++tex_coord_index) {
			const auto tex_coord = mesh->mTextureCoords[tex_index][tex_coord_index];
			out.texCoords[tex_index][tex_coord_index] = (XMFLOAT3(tex_coord.x, tex_coord.y, tex_coord.z));
		}
	}
}
