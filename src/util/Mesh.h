#pragma once
#include <memory>
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

namespace util
{
	class Mesh
	{
	private:
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;

		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;

		void SetupMesh();
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
		~Mesh();

		void Draw();
	};
}
