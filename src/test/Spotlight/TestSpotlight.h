#pragma once
#include "glm/vec3.hpp"
#include "test/Test.h"

class Shader;
class Camera;
class Object;

namespace test
{
	class TestSpotlight : public Test
	{
	private:
		std::unique_ptr<Object> m_Obj;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;

		glm::vec3 m_ObjPosition;
		glm::vec3 m_LightPosition;
		glm::vec3 m_LightDir;
		float m_CutoffAngle_Inner;
		float m_CutoffAngle_Outer;
		float m_Intensity;

	public:
		TestSpotlight();
		~TestSpotlight();

		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}
