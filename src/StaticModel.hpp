#pragma once
#include <vector>
#include "SceneNode.hpp"
#include "Model.hpp"
#include "Mesh.hpp"


/**
	 * \brief Helper class used to draw a single mesh out of a model.
	 * \notes If you want to draw a set of static meshes, use a StaticModel instead.
	 */
	class StaticMesh : public Mesh
	{
	private:
	protected:
	public:
		//Use instead of Mesh.SceneNode
		SceneNode rootTransform;
		static StaticMesh ImportModel(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
		static StaticMesh ImportModel(uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
		static StaticMesh ImportModel_FBX(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
		static StaticMesh ImportModel_FBX(uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
	};
	class StaticModel : public Model
	{
	private:
	protected:
	public:
		std::vector<Mesh> meshes;
		SceneNode rootNode;
		static StaticModel ImportModel(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
		static StaticModel ImportModel(uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
		static StaticModel ImportModel_FBX(const char* fileName, bool flipUVs = false, bool flipWindingOrder = false);
		static StaticModel ImportModel_FBX(uint8_t* uint8_tArray, size_t arraySize, bool flipUVs = false, bool flipWindingOrder = false);
	};
