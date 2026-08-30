#pragma once
#include "glm/vec3.hpp"
#include "test/Test.h"
#include "util/Mesh.h"
class Shader;
class Object;
class Camera;

namespace test
{
	class TestPBR : public Test
	{
	private:
		std::shared_ptr<util::Mesh> m_SphereMesh;
		unsigned int m_SphereRow = 10;
		unsigned int m_SphereColumn = 10;
		float m_SphereSpacing = 2.5f;
		glm::vec3 m_SphereAlbedo;
		glm::vec3 m_SphereTranslation;
		std::vector<Object> m_Spheres;
		std::vector<glm::vec3> m_LightPos;
		std::vector<glm::vec3> m_LightColor;
		std::unique_ptr<Shader> m_SphereShader;
		std::unique_ptr<Camera> m_Camera;

		unsigned int m_QuadVAO, m_QuadVBO;
		unsigned int m_fbo, m_TextureColorBuffer, m_rbo;
		std::unique_ptr<Shader> m_QuadShader;

		bool bHDR = true;
		bool bHDRKeyPressed = false;

	public:
		TestPBR();
		~TestPBR();
		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;



	};
}

