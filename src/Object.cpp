#include "../Object.h"
#include "util/ObjParser.h"

Object::Object(const std::string& filepath,
	std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glm::vec3 world_position,
    glm::vec3 rotation, glm::vec3 scale)
{
	util::ObjParser::ParseOBJ(filepath, vertices, indices);
	m_Mesh = std::make_unique<util::Mesh>(vertices, indices);

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
