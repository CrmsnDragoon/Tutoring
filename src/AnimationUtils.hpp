#pragma once
#include <string>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "SceneNode.hpp"
#include "Mesh.hpp"
#pragma region printFuncs

std::string OpToStr(aiTextureOp op);

std::string MapModeToStr(aiTextureMapMode op);

std::string TextureTypeToString(aiTextureType type);

void PrintMaterial(aiMaterial* material);

std::string AnimBehaviourToString(aiAnimBehaviour behaviour);
void PrintSkeleton(const SceneNode* sceneNode, const int skeletonDepth = 0);

void PrintAnimation(const aiAnimation* anim);

#pragma endregion
uint32_t AssimpImportFlags(const bool flipUVs, const bool flipWindingOrder);

void ImportMesh(Mesh& out, const aiMesh* mesh, const aiScene* scene);
