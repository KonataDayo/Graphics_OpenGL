#pragma once
#include "../Test.h"

class Camera;
class Shader;

namespace test
{
	class TestEnvironmentMapping : public Test
	{
	private:
		std::vector<std::string> m_TexturesFaces;
		unsigned int m_CubeVAO, m_SkyboxVAO;
		unsigned int m_CubeVBO, m_SkyboxVBO;
		unsigned int m_SkyboxTexture;
		std::unique_ptr<Shader> m_CubeShader, m_SkyboxShader;
		std::unique_ptr<Camera> m_Camera;
		float m_Reflectance, m_RefractiveIndex;

	public:
		TestEnvironmentMapping();
		~TestEnvironmentMapping();
		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;

	};

}
