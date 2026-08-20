#include "TestSimpleLightScene.h"
#include "../../../IndexBuffer.h"
#include "../../../Renderer.h"
#include "../../../Shader.h"
#include "../../../VertexArray.h"
#include "../../../VertexBuffer.h"
#include "../../../VertexBufferLayout.h"
#include "glm/gtx/transform.hpp"
#include "ImGui/imgui.h"
#include "util/Camera.h"

test::TestSimpleLightScene::TestSimpleLightScene()
	: m_AmbientStrength(0.1f), m_DiffuseCoefficient(0.4f),m_Intensity(20),m_SpecularLobeSize(16.f),
	m_LightColor{1.f,1.f,1.f,1.f}, m_ObjColor{0.2f, 0.3f, 0.8f, 1.f}, 
	m_ObjPosition(0.f,-1.f,-1.f), m_LightPosition(4.f,2.f,-4.f),
	m_SpecularCoefficient(0.7f)
{
    float vertices[] =
    {
        // position              // normal

        // Front (+Z)
        -0.5f,-0.5f, 0.5f,       0,0,1,
         0.5f,-0.5f, 0.5f,       0,0,1,
         0.5f, 0.5f, 0.5f,       0,0,1,
        -0.5f, 0.5f, 0.5f,       0,0,1,


        // Back (-Z)
        -0.5f,-0.5f,-0.5f,       0,0,-1,
         0.5f,-0.5f,-0.5f,       0,0,-1,
         0.5f, 0.5f,-0.5f,       0,0,-1,
        -0.5f, 0.5f,-0.5f,       0,0,-1,


        // Left (-X)
        -0.5f,-0.5f,-0.5f,      -1,0,0,
        -0.5f,-0.5f, 0.5f,      -1,0,0,
        -0.5f, 0.5f, 0.5f,      -1,0,0,
        -0.5f, 0.5f,-0.5f,      -1,0,0,


        // Right (+X)
         0.5f,-0.5f, 0.5f,       1,0,0,
         0.5f,-0.5f,-0.5f,       1,0,0,
         0.5f, 0.5f,-0.5f,       1,0,0,
         0.5f, 0.5f, 0.5f,       1,0,0,


         // Top (+Y)
         -0.5f,0.5f, 0.5f,        0,1,0,
          0.5f,0.5f, 0.5f,        0,1,0,
          0.5f,0.5f,-0.5f,        0,1,0,
         -0.5f,0.5f,-0.5f,        0,1,0,


         // Bottom (-Y)
         -0.5f,-0.5f,-0.5f,       0,-1,0,
          0.5f,-0.5f,-0.5f,       0,-1,0,
          0.5f,-0.5f, 0.5f,       0,-1,0,
         -0.5f,-0.5f, 0.5f,       0,-1,0
    };

    unsigned int indices[] =
    {
        // Front
        0,1,2,
        2,3,0,

        // Back
        4,6,5,
        6,4,7,

        // Left
        8,9,10,
        10,11,8,

        // Right
        12,13,14,
        14,15,12,

        // Top
        16,17,18,
        18,19,16,

        // Bottom
        20,21,22,
        22,23,20
    };

    GLCall(glEnable(GL_DEPTH_TEST));

	m_ObjVAO = std::make_unique<VertexArray>();
    m_ObjVAO->Bind();
	m_CubeVBO = std::make_unique<VertexBuffer>(vertices, 144 * sizeof(float));
    m_CubeIBO = std::make_unique<IndexBuffer>(indices, 36);
    m_ObjShader = std::make_unique<Shader>("res/shaders/BlinnPhong.shader");
    VertexBufferLayout objLayout;
    objLayout.Push<float>(3); // positions
    objLayout.Push<float>(3); // normals
    m_ObjVAO->AddBuffer(*m_CubeVBO, objLayout);

	m_LightPointVAO = std::make_unique<VertexArray>();
    m_LightPointVAO->Bind();
    m_CubeVBO->Bind();
    m_CubeIBO->Bind();
    m_LightPointShader = std::make_unique<Shader>("res/shaders/LightSource.shader");
    VertexBufferLayout lightLayout;
    lightLayout.Push<float>(3); // positions
    lightLayout.Push<float>(3); // normals
    m_LightPointVAO->AddBuffer(*m_CubeVBO, lightLayout);

    m_Camera = std::make_unique<Camera>();
}

test::TestSimpleLightScene::~TestSimpleLightScene()
{
}

void test::TestSimpleLightScene::OnRender()
{
	Test::OnRender();
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 500.f);
    glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,-3.f));
    view = m_Camera->GetLookAtMatrix() * view;

	// Light
    Renderer lightRenderer;
    glm::mat4 light_model = glm::translate(glm::mat4(1.f), m_LightPosition);
    glm::mat4 light_mvp = proj * view * light_model;
    m_LightPointShader->Bind();
    m_LightPointShader->SetUniform4f("u_LightColor",m_LightColor[0], m_LightColor[1], m_LightColor[2], m_LightColor[3]);
    m_LightPointShader->SetUniformMat4f("u_MVP",light_mvp);
	lightRenderer.Draw(*m_LightPointVAO, *m_CubeIBO, *m_LightPointShader);

    // Object
    Renderer objRenderer;
    glm::mat4 obj_model = glm::translate(glm::mat4(1.f), m_ObjPosition);
    glm::mat4 obj_mvp = proj * view * obj_model;
    m_ObjShader->Bind();
    m_ObjShader->SetUniformMat4f("u_Model", obj_model);
    m_ObjShader->SetUniformMat4f("u_MVP", obj_mvp);
    m_ObjShader->SetUniform4f("u_LightColor", m_LightColor[0], m_LightColor[1], m_LightColor[2], m_LightColor[3]);
    m_ObjShader->SetUniform4f("u_ObjColor", m_ObjColor[0], m_ObjColor[1], m_ObjColor[2], m_ObjColor[3]);
    m_ObjShader->SetUniform1f("u_AmbientStrength", m_AmbientStrength);
    m_ObjShader->SetUniform1f("u_DiffuseCoefficient", m_DiffuseCoefficient);
	m_ObjShader->SetUniform3f("u_LightPosition", m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
    m_ObjShader->SetUniform1f("u_Intensity", m_Intensity);
    m_ObjShader->SetUniform1f("u_SpecularCoefficient", m_SpecularCoefficient);
    m_ObjShader->SetUniform1f("u_SpecularSize", m_SpecularLobeSize);
    glm::vec3 cameraLocation = m_Camera->GetCameraLocation();
    m_ObjShader->SetUniform3f("u_CameraLocation",cameraLocation.x, cameraLocation.y, cameraLocation.z);
	objRenderer.Draw(*m_ObjVAO, *m_CubeIBO, *m_ObjShader);

}

void test::TestSimpleLightScene::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
    m_Camera->ProcessInput(window, deltaTime);
}

void test::TestSimpleLightScene::OnImGuiRender()
{
	Test::OnImGuiRender();
    ImGui::SliderFloat3("Light Position", &m_LightPosition.x, -6.f,6.f);
    ImGui::SliderFloat3("Obj Position", &m_ObjPosition.x, -6.f, 6.f);
    ImGui::ColorEdit4("Light Color", m_LightColor);
    ImGui::ColorEdit4("Object Color", m_ObjColor);
    ImGui::SliderFloat("Ambient", &m_AmbientStrength, 0.f, 0.3f);
    ImGui::SliderFloat("Diffuse Strength", &m_DiffuseCoefficient, 0.f, 1.0f);
	ImGui::SliderFloat("Light Intensity", &m_Intensity,0.f, 300.f);
    ImGui::SliderFloat("Specular Lobe Size", &m_SpecularLobeSize, 1, 128);
    ImGui::SliderFloat("Specular Coefficient/Shininess", &m_SpecularCoefficient, 0.0f, 1.f);
}
