#include "TestFOV.h"
#include "ImGui/imgui.h"
#include "../Shader.h"
#include "../Renderer.h"
#include "glm/gtx/transform.hpp"

test::TestFOV::TestFOV()
	: m_FOV_Rad(90)
{
    float vertices[] = {
        // Front
        -1.0f, -1.0f, -2.0f,   // 0
         1.0f, -1.0f, -2.0f,   // 1
         1.0f,  1.0f, -2.0f,   // 2
        -1.0f,  1.0f, -2.0f,   // 3

        // Back
        -1.0f, -1.0f, -8.0f,   // 4
         1.0f, -1.0f, -8.0f,   // 5
         1.0f,  1.0f, -8.0f,   // 6
        -1.0f,  1.0f, -8.0f    // 7
    };

    unsigned int indices[] = {
        // Front
        0, 1, 2,
        2, 3, 0,

        // Back
        5, 4, 7,
        7, 6, 5,

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

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int),indices,GL_STATIC_DRAW);

    m_Shader = std::make_unique<Shader>("res/shaders/ThreeDimension.shader");
    m_Shader->Bind();

    GLCall(glEnable(GL_DEPTH_TEST));
}

test::TestFOV::~TestFOV()
{
}

void test::TestFOV::OnRender()
{
	Test::OnRender();

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    //glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 100.f);
    glm::mat4 proj = glm::perspectiveFov(glm::radians(m_FOV_Rad),1920.f, 1080.f, 0.1f, 100.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.0f, -2.0f));
    glm::mat4 model = glm::rotate(glm::mat4(1.f), glm::radians(30.f), glm::vec3(5,3,1));
    glm::mat4 mvpA = proj * view * model;
    m_Shader->SetUniformMat4f("u_MVP",mvpA);

    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
	GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

    glm::mat4 modelB = glm::translate(glm::mat4(1.f), glm::vec3(-2.f, 0.5f, -5.f));
    modelB = glm::rotate(modelB,glm::radians(45.f),glm::vec3(3.f, 2.f, 1.f));
	glm::mat4 mvpB = proj * view * modelB;
    m_Shader->SetUniformMat4f("u_MVP",mvpB);

    GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
}

void test::TestFOV::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
}

void test::TestFOV::OnImGuiRender()
{
	Test::OnImGuiRender();
    ImGui::SliderFloat("FOV/degree", &m_FOV_Rad, 1.f, 180.f);
}
