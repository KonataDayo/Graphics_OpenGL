#pragma once
#include <glm/detail/type_mat.hpp>
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

namespace util
{
	struct Transform
	{
		glm::vec3 WorldPosition;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 GetModelMatrix() const
		{
			glm::mat4 model = glm::translate(glm::mat4(1.f), WorldPosition);

            model = glm::rotate(
                model,
                glm::radians(Rotation.x),
                glm::vec3(1, 0, 0)
            );

            model = glm::rotate(
                model,
                glm::radians(Rotation.y),
                glm::vec3(0, 1, 0)
            );

            model = glm::rotate(
                model,
                glm::radians(Rotation.z),
                glm::vec3(0, 0, 1)
            );

			model = glm::scale(model, Scale);

            return model;
		}
	};
}
