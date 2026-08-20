#pragma once

#include "Test.h"
#include <memory>
#include "../VertexArray.h"
#include "../Shader.h"
#include "../IndexBuffer.h"
#include "../../Texture.h"

namespace test
{
	class TestTexture2D : public Test
	{
	public:
		TestTexture2D();
		~TestTexture2D() override;

		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		glm::vec3 m_translationA, m_translationB;
		glm::mat4 m_Rotation;
		float m_RotateDegree_Rad;
		glm::mat4 proj, view;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
	};
}
