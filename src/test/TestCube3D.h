#pragma once
#include "Test.h"
#include <memory>

#include "glm/vec3.hpp"

class Shader;
class IndexBuffer;
class VertexBuffer;
class VertexArray;

namespace test
{
	class TestCube3D : public Test
	{
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<IndexBuffer> m_IBO;
		std::unique_ptr<Shader> m_Shader;
		float u_Color[4];
		glm::vec3 m_CubePositions[10];
		float m_DegreePerSec;
		float m_MoveSpeed;
	public:
		TestCube3D();
		~TestCube3D();

		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}
