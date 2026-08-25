#pragma once
#include "test/Test.h"

class Camera;
class Shader;
class IndexBuffer;
class VertexArray;

namespace test
{
	class TestFramebuffer : public Test
	{
	private:
		unsigned int m_FBO, m_TextureColorBuffer, m_TextureRenderBuffer;
		unsigned int m_CubeTexture, m_PlaneTexture;
		unsigned int m_CubeVAO, m_PlaneVAO, m_QuadVAO;
		unsigned int m_CubeVBO, m_PlaneVBO, m_QuadVBO;

		std::unique_ptr<Shader> m_CubeShader;
		std::unique_ptr<Shader> m_PlaneShader;
		std::unique_ptr<Shader> m_QuadShader;

		std::unique_ptr<Camera> m_Camera;


	public:
		TestFramebuffer();
		~TestFramebuffer();

		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;


	};
}
