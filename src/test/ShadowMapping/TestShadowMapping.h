#pragma once
#include "../Test.h"
#include "glm/vec3.hpp"
#define TOTAL_CUBE 3
#define DEPTH_MAP_WIDTH 1024
#define DEPTH_MAP_HEIGHT 1024

class Shader;
class Camera;


namespace test
{
	class TestShadowMapping : public Test
	{
	private:
		unsigned int m_cubeVAO, m_quadVAO, m_planeVAO;
		unsigned int m_cubeVBO, m_quadVBO, m_planeVBO;
		unsigned int m_texture;
		unsigned int m_depthMap, m_fbo;
		std::unique_ptr<Shader> m_shader; // for depth map
		std::unique_ptr<Shader> m_objShader;
		std::unique_ptr<Shader> m_quadShader;
		std::unique_ptr<Camera> m_camera;
		std::vector<glm::vec3> m_cubeModelMat;
		glm::vec3 m_lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		std::vector<glm::vec3> m_cubeTranslation;


	public:
		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;
		TestShadowMapping();
		~TestShadowMapping();


	};
}
