#include "../Object.h"

#include <iostream>

#include "util/ObjParser.h"

Object::Object(const std::string& filepath,glm::vec3 world_position,glm::vec3 rotation, glm::vec3 scale)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	util::ObjParser::ParseOBJ(filepath, vertices, indices);
	m_Mesh = std::make_shared<util::Mesh>(vertices, indices);

	m_Transform.WorldPosition = world_position;
	m_Transform.Rotation = rotation;
	m_Transform.Scale = scale;
}

Object::Object(std::shared_ptr<util::Mesh> mesh, glm::vec3 world_position, glm::vec3 rotation, glm::vec3 scale)
	: m_Mesh(std::move(mesh))
{
	if (!m_Mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::cout<<"[Object] failed to find mesh"<<std::endl;
		util::ObjParser::ParseOBJ("res/meshes/pbr-sphere.obj", vertices, indices);
		m_Mesh = std::make_shared<util::Mesh>(vertices, indices);
	}

	m_Transform.WorldPosition = world_position;
	m_Transform.Rotation = rotation;
	m_Transform.Scale = scale;
}

Object::~Object()
{

}

glm::mat4 Object::GetModelMatrix() const
{
	return m_Transform.GetModelMatrix();
}

void Object::SetTransform(util::Transform transform)
{
	m_Transform = transform;
}

void Object::DrawObject()
{
	m_Mesh->Draw();
}
