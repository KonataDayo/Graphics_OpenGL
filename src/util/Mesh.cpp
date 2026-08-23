#include "Mesh.h"
#include "../../VertexBufferLayout.h"
#include <cstddef>

#include "../../Shader.h"

void util::Mesh::SetupMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// texture coordinate
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

	glBindVertexArray(0);
}

util::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
	: m_Vertices(vertices), m_Indices(indices)
{
	SetupMesh();
}

util::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
	const std::vector<FTexture>& textures)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
	SetupMesh();
}

util::Mesh::~Mesh()
{

}

void util::Mesh::Draw()
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void util::Mesh::Draw_Texture(Shader& shader)
{
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;
	for (unsigned int i = 0 ;i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		FTexture tex = m_Textures[i];
		std::string num;
		if (tex.type == "texture_diffuse")
			num = std::to_string(diffuseNum++);
		else if (tex.type == "texture_specular")
			num = std::to_string(specularNum++);
		shader.SetUniform1i(("material." + tex.type + num).c_str(),i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
	}
	glActiveTexture(0);
	Draw();
}
