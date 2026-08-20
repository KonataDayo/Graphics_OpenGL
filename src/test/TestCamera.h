#pragma once
#include "Test.h"
#include <memory>
#include <glm/detail/type_mat.hpp>

#include "glm/vec3.hpp"

class Camera;
class Shader;
class IndexBuffer;
class VertexBuffer;
class VertexArray;

namespace test
{
	class TestCamera : public Test
	{
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<IndexBuffer> m_IBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;
		float u_Color[4];
		glm::vec3 m_CubePositions[10];
		float m_DegreePerSec;
		float m_MoveSpeed;
	public:
		TestCamera();
		~TestCamera();

		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}
