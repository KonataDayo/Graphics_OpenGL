#pragma once
#pragma once
#include "glm/vec3.hpp"
#include "glm/detail/type_vec4.hpp"
#include "test/Test.h"
#include "util/Mesh.h"
class Shader;
class Object;
class Camera;

namespace test
{
	class TestTexturePBR : public Test
	{
	private:
		std::shared_ptr<util::Mesh> m_SphereMesh;
		unsigned int m_SphereRow = 1;
		unsigned int m_SphereColumn = 1;
		float m_SphereSpacing = 10.f;
		glm::vec3 m_SphereTranslation;
		std::vector<Object> m_Spheres;
		std::vector<glm::vec3> m_LightPos;
		std::vector<glm::vec3> m_LightColor;
		std::unique_ptr<Shader> m_SphereShader;
		std::unique_ptr<Camera> m_Camera;

		unsigned int m_QuadVAO, m_QuadVBO;
		unsigned int m_fbo, m_TextureColorBuffer, m_rbo;
		std::unique_ptr<Shader> m_QuadShader;

		unsigned int m_TexAlbedo, m_TexNormal, m_TexRoughness, m_TexMetallic, m_TexAO;

		bool bHDR = true;
		bool bHDRKeyPressed = false;

	public:
		TestTexturePBR();
		~TestTexturePBR();
		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;



	};
}

