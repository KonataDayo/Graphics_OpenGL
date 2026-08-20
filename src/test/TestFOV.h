#pragma once
#include "Test.h"
#include "../VertexArray.h"
#include "../../IndexBuffer.h"
#include "../../VertexBuffer.h"
#include <memory>
class Shader;

namespace test
{
	class TestFOV: public Test
	{
	private:
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		std::unique_ptr<Shader> m_Shader;
		float m_FOV_Rad;
	public:
		TestFOV();
		~TestFOV();

		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;

	};
}
