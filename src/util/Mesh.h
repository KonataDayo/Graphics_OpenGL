#pragma once
#include <memory>
#include <string>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/detail/type_vec2.hpp"

class Shader;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

struct FTexture
{
	unsigned int id;
	std::string type;
	std::string path;
};

namespace util
{
	class Mesh
	{
	private:
		unsigned int m_VAO, m_VBO, m_IBO;

		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
		std::vector<FTexture> m_Textures;

		void SetupMesh();
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<FTexture>& textures);
		~Mesh();

		void Draw();
		void Draw_Texture(Shader& shader);
	};
}
