#include "TestTexture2D.h"
#include "../Renderer.h"
#include "../VertexBufferLayout.h"
#include "../VertexBuffer.h"
#include "../Texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/detail/type_mat.hpp>
#include "ImGui/imgui.h"

test::TestTexture2D::TestTexture2D()
	: m_translationA(200, 200 ,0), m_translationB(400, 200 ,0),
	proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.f, -1.0f, 1.0f)), view(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
	m_Rotation(glm::mat4(1.0f)), m_RotateDegree_Rad(0.f)
{
    float positions[] = {
          500.0f, -500.0f,  1.0f,  0.0f, // 0
         -500.0f, -500.5f,  0.0f,  0.0f, // 1
          500.0f,  500.0f,  1.0f,  1.0f, // 2
         -500.0f,  500.0f,  0.0f,  1.0f  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 1
    };

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

	m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
    m_Texture = std::make_unique<Texture>("res/textures/wave.png");
    m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");

	VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    m_VAO->AddBuffer(*m_VertexBuffer, layout);
    m_VAO->Unbind();

    m_VertexBuffer->Unbind();

    m_Texture->Bind();

    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
    m_Shader->SetUniform1i("u_Texture", 0);

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
    m_IndexBuffer->Unbind();
}

test::TestTexture2D::~TestTexture2D()
{

}

void test::TestTexture2D::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
}

void test::TestTexture2D::OnRender()
{
	Test::OnRender();
    Renderer renderer;
    m_Texture->Bind();
    m_VAO->Bind();
    m_IndexBuffer->Bind();
    m_Shader->Bind();

    m_Rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_RotateDegree_Rad), glm::vec3(0, 0, 1.0));

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationA);
        glm::mat4 mvp = proj * view * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationB) * m_Rotation;
        glm::mat4 mvp = proj * view * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }

}

void test::TestTexture2D::OnImGuiRender()
{
	Test::OnImGuiRender();
    ImGui::SliderFloat3("translation A", &m_translationA.x, 0.0f, 1920.0f);
    ImGui::SliderFloat3("translation B", &m_translationB.x, 0.0f, 1920.0f);
    ImGui::SliderFloat("Rotation", &m_RotateDegree_Rad, -360.f, 360.f);
}
