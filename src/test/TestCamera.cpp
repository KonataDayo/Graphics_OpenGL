#include "TestCamera.h"
#include <GLFW/glfw3.h>
#include "ImGui/imgui.h"
#include "../VertexArray.h"
#include "../../IndexBuffer.h"
#include "../../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../Shader.h"
#include "glm/gtx/rotate_vector.hpp"
#include "util/Camera.h"

test::TestCamera::TestCamera()
    : u_Color{ 0.2f, 0.3f, 0.8f, 1.0f }, m_DegreePerSec(15.f), m_MoveSpeed(0.f),
    m_CubePositions{
    glm::vec3(0.0f,  0.0f,  -5.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f) }
{
    float vertices[] = {
        -0.5f, -0.5f,  0.5f,// 0
         0.5f, -0.5f,  0.5f,// 1
         0.5f,  0.5f,  0.5f,// 2
        -0.5f,  0.5f,  0.5f,// 3
        -0.5f, -0.5f, -0.5f,// 4
         0.5f, -0.5f, -0.5f,// 5
         0.5f,  0.5f, -0.5f,// 6
        -0.5f,  0.5f, -0.5f // 7
    };

    unsigned int indices[] = {
        // Front
        0, 1, 2,
        2, 3, 0,
        // Back
        4, 6, 5,
        6, 4, 7,
        // Left
        4, 0, 3,
        3, 7, 4,
        // Right
        1, 5, 6,
        6, 2, 1,
        // Top
        3, 2, 6,
        6, 7, 3,
        // Bottom
        4, 5, 1,
        1, 0, 4
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(vertices, 24 * sizeof(float));
    m_IBO = std::make_unique<IndexBuffer>(indices, 36);
    m_Shader = std::make_unique<Shader>("res/shaders/ThreeDimension.shader");

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

    VertexBufferLayout layout;
    layout.Push<float>(3);

    m_VAO->AddBuffer(*m_VBO, layout);
    m_VAO->Bind();
    m_IBO->Bind();
    m_Shader->Bind();

    GLCall(glEnable(GL_DEPTH_TEST));

    m_Camera = std::make_unique<Camera>();
}

test::TestCamera::~TestCamera()
{
}

void test::TestCamera::OnUpdate(GLFWwindow* window, float deltaTime)
{
    Test::OnUpdate(window, deltaTime);
    m_Camera->ProcessInput(window,deltaTime);
}

void test::TestCamera::OnRender()
{
    Test::OnRender();
    Renderer renderer;
    float fov = m_Camera->GetFOV();
    glm::mat4 proj = glm::perspective(glm::radians(fov), 1920.f / 1080.f, 0.1f, 100.f);
	glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.0f, -2.0f));
    view = m_Camera->GetLookAtMatrix() * view;
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    m_Shader->SetUniform4f("u_Color", u_Color[0], u_Color[1], u_Color[2], u_Color[3]);
    for (unsigned i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.f), m_CubePositions[i]);
        float angle = 20.f * i + (float)glfwGetTime() * m_DegreePerSec;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3, 0.5f));
        glm::mat4 mvp = proj * view * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }
}

void test::TestCamera::OnImGuiRender()
{
    Test::OnImGuiRender();
    ImGui::ColorEdit4("Color", u_Color);
    ImGui::SliderFloat("degree/s", &m_DegreePerSec, -60, 60);
}

