#pragma once
#include <string>
#include <vector>
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(const char* path, const std::string& directory);

namespace util
{
	class Model
	{
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;

		void LoadModel(const std::string& filepath);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<FTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	public:
		Model(const std::string& filepath);
		~Model();
		void Draw(Shader& shader);
	};
}
