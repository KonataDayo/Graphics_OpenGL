#pragma once
#include <GLFW/glfw3native.h>

#include "../Test.h"
#include "util/Mesh.h"

#define CUBEMAP_RESOLUTION 2048

class Object;
class Shader;
class Camera;

namespace test
{
	class TestIBL : public Test
	{
	private:
		std::vector<Object> m_Spheres;
		std::shared_ptr<util::Mesh> m_SphereMesh;
		unsigned int m_SphereRow = 10;
		unsigned int m_SphereColumn = 10;
		float m_SphereSpacing = 2.5f;
		glm::vec3 m_SphereTranslation;
		std::unique_ptr<Shader> m_SphereShader;
		std::unique_ptr<Camera> m_Camera;

		// Environment map
		unsigned int m_captureFBO, m_capturedCubemap, m_captureRBO;
		std::unique_ptr<Shader> m_FromEquirectangleToCubemapShader;
		unsigned int m_HDRI;
		unsigned int m_cubeMap;
		unsigned int m_skyboxVAO, m_skyboxVBO;
		std::unique_ptr<Shader> m_skyboxShader;

		// Post-processing
		unsigned int m_quadVAO;
		unsigned int m_quadVBO;
		unsigned int m_quadFBO;
		unsigned int m_quadRBO;
		unsigned int m_quadTexture;
		std::unique_ptr<Shader> m_quadShader;

	public:
		TestIBL();
		~TestIBL();
		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;



	};
}
