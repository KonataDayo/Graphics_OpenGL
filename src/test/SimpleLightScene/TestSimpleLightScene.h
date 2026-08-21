#pragma once
#include "test/Test.h"
#include <memory>

#include "../../../Object.h"
#include "glm/detail/type_vec3.hpp"
#include "glm/detail/type_vec4.hpp"
#include "util/Mesh.h"

class Camera;
class VertexBuffer;
class Shader;
class IndexBuffer;
class VertexArray;

namespace test
{
	class TestSimpleLightScene : public Test
	{
	private:
		std::unique_ptr<VertexArray> m_ObjVAO;
		std::unique_ptr<Shader> m_ObjShader;

		std::unique_ptr<util::Mesh> m_Teapot;
		std::unique_ptr<Object> m_TeapotObj;
		std::unique_ptr<Shader> m_TeapotShader;

		glm::vec3 m_TeapotWorldPosition;
		glm::vec3 m_TeapotRotation;
		glm::vec3 m_TeapotScale;

		std::unique_ptr<VertexArray> m_LightPointVAO;
		std::unique_ptr<Shader> m_LightPointShader;

		std::unique_ptr<IndexBuffer> m_CubeIBO;
		std::unique_ptr<VertexBuffer> m_CubeVBO;

		std::unique_ptr<Camera> m_Camera;

		float m_LightColor[4];
		float m_ObjColor[4];

		glm::vec3 m_LightPosition;
		glm::vec3 m_ObjPosition;

		float m_AmbientStrength;
		float m_DiffuseCoefficient;
		float m_Intensity;
		float m_SpecularLobeSize;
		float m_SpecularCoefficient;

	public:
		TestSimpleLightScene();
		~TestSimpleLightScene();

		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;

	};
}
