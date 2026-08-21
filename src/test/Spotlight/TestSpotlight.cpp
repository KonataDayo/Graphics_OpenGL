#include "TestSpotlight.h"
#include "util/Camera.h"
#include "../../../Object.h"
#include "../../../Renderer.h"
#include "../../../Shader.h"
#include "ImGui/imgui.h"

test::TestSpotlight::TestSpotlight()
	:
	m_CutoffAngle_Inner(30.f),
	m_CutoffAngle_Outer(45.f),
	m_Intensity(30.f),
	m_LightDir(0.f, -1.f, 0.f),
	m_LightPosition(0.f, 2.f, -2.f),
	m_ObjPosition(0.f, 0.f, 0.f)
{
	m_Obj = std::make_unique<Object>(
		"res/meshes/utah_teapot_res16.obj",
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f,1.f,1.f)
	);
	m_Camera = std::make_unique<Camera>();
	m_Shader = std::make_unique<Shader>("res/shaders/Spotlight.shader");
	m_Shader->Bind();
	glEnable(GL_DEPTH_TEST);
}

test::TestSpotlight::~TestSpotlight()
{

}

void test::TestSpotlight::OnRender()
{
	Test::OnRender();
	glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f/1080.f, 0.1f, 500.f);
	glm::mat4 view = m_Camera->GetLookAtMatrix() * glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,-3.f));
	glm::mat4 model = glm::translate(glm::mat4(1.f), m_ObjPosition);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer renderer;
	m_Shader->SetUniformMat4f("u_proj", proj);
	m_Shader->SetUniformMat4f("u_view", view);
	m_Shader->SetUniformMat4f("u_model", model);
	m_Shader->SetUniform1f("u_innerCutoff", glm::radians(m_CutoffAngle_Inner));
	m_Shader->SetUniform1f("u_outerCutoff", glm::radians(m_CutoffAngle_Outer));
	m_Shader->SetUniform1f("u_intensity", m_Intensity);
	m_Shader->SetUniform3f("u_objPos",m_ObjPosition.x, m_ObjPosition.y, m_ObjPosition.z);
	m_Shader->SetUniform3f("u_lightPos",m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
	m_Shader->SetUniform3f("u_lightDir",m_LightDir.x, m_LightDir.y, m_LightDir.z);
	glm::vec3 camPos = m_Camera->GetCameraLocation();
	m_Shader->SetUniform3f("u_cameraLocation",camPos.x, camPos.y, camPos.z);

	renderer.DrawObject(*m_Obj, *m_Shader);
}

void test::TestSpotlight::OnImGuiRender()
{
	Test::OnImGuiRender();
	ImGui::SliderFloat3("Light Direction", &m_LightDir.x, -6.f,6.f);
	ImGui::SliderFloat3("Light Position", &m_LightPosition.x, -6.f, 6.f);
	ImGui::SliderFloat3("Object Position", &m_ObjPosition.x, -6.f, 6.f);
	ImGui::SliderFloat("Inner Cone Cut-off Angle", &m_CutoffAngle_Inner, 1.f, 60.f);
	ImGui::SliderFloat("Outer Cone Cut-off Angle", &m_CutoffAngle_Outer, m_CutoffAngle_Inner, 80.f);
	ImGui::SliderFloat("Light Intensity", &m_Intensity, 0.f, 300.f);
}

void test::TestSpotlight::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
	m_Camera->ProcessInput(window, deltaTime);
}
