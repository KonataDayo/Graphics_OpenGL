#pragma once
#include <memory>
#include <string>
#include <vector>
#include "util/Mesh.h"
#include "util/Transform.h"

class Object
{
private:
	std::unique_ptr<util::Mesh> m_Mesh;
	util::Transform m_Transform;

public:
	Object(const std::string& filepath, glm::vec3 world_position, glm::vec3 rotation, glm::vec3 scale);
	~Object();

	glm::mat4 GetModelMatrix() const;

	void SetTransform(util::Transform transform);

	void DrawObject();
};
