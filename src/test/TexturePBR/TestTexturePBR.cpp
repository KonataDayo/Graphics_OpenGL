#include "TestTexturePBR.h"

#include "../../../Object.h"
#include "../../../Shader.h"
#include "../../../Texture.h"
#include "glm/gtx/string_cast.hpp"
#include "ImGui/imgui.h"
#include "util/Camera.h"
#include "util/ObjParser.h"

test::TestTexturePBR::TestTexturePBR()
{
    // Light Properties
    m_LightPos.emplace_back(-10.0f, 10.0f, 10.0f);
    m_LightPos.emplace_back(10.0f, 10.0f, 10.0f);
    m_LightPos.emplace_back(-10.0f, -10.0f, 10.0f);
    m_LightPos.emplace_back(10.0f, -10.0f, 10.0f);

    m_LightColor.emplace_back(300.0f, 300.0f, 300.0f);
    m_LightColor.emplace_back(300.0f, 300.0f, 300.0f);
    m_LightColor.emplace_back(300.0f, 300.0f, 300.0f);
    m_LightColor.emplace_back(300.0f, 300.0f, 300.0f);


    // Spheres
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    util::ObjParser::ParseOBJ("res/meshes/RustyCan_Tri.obj", vertices, indices);
    std::cout << "vertices: " << vertices.size() << std::endl;
    std::cout << "indices: " << indices.size() << std::endl;
    m_SphereMesh = std::make_shared<util::Mesh>(vertices, indices);
    for (unsigned int i = 0; i < m_SphereRow; i++)
    {
        for (unsigned int j = 0; j < m_SphereColumn; j++)
        {
            glm::vec3 position, rotation, scale;
            position = glm::vec3((float)(i * m_SphereSpacing), (float)(j * -m_SphereSpacing), 0.f);
            rotation = glm::vec3(0.f, 0.f, 0.f);
            scale = glm::vec3(4.f, 4.f, 4.f);
            m_Spheres.emplace_back(m_SphereMesh, position, rotation, scale);
        }
    }

    m_SphereShader = std::make_unique<Shader>("res/shaders/PBR_Texture.shader");
    m_QuadShader = std::make_unique<Shader>("res/shaders/SimpleHDR.shader");
    m_Camera = std::make_unique<Camera>();
    glEnable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "HDR framebuffer is NOT complete!\n";
    else
        std::cout << "HDR framebuffer is complete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_QuadVAO);
    glBindVertexArray(m_QuadVAO);

    glGenBuffers(1, &m_QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    ////////////////////////////////////////////////////////////
    // Texture
    ///////////////////////////////////////////////////////////
    m_TexAlbedo = loadTexture("res/textures/rusty_tin_can/RustyCan_BaseColor.png");
    m_TexRoughness = loadTexture("res/textures/rusty_tin_can/RustyCan_Roughness.png");
    m_TexMetallic = loadTexture("res/textures/rusty_tin_can/RustyCan_Metallic.png");
    m_TexNormal = loadTexture("res/textures/rusty_tin_can/RustyCan_Normal.png");

}

test::TestTexturePBR::~TestTexturePBR()
{
}

void test::TestTexturePBR::OnRender()
{
    Test::OnRender();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 1000.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 cameraDir = m_Camera->GetLookAtMatrix();
    view = cameraDir * view;

    m_SphereShader->Bind();
    for (unsigned int i = 0; i < m_Spheres.size(); i++)
    {
        glm::mat4 model = m_Spheres[i].GetModelMatrix();
        model = glm::translate(model, m_SphereTranslation);
        glm::mat4 sphereMVP = proj * view * model;
        m_SphereShader->SetUniformMat4f("proj", proj);
        m_SphereShader->SetUniformMat4f("view", view);
        m_SphereShader->SetUniformMat4f("model", model);
        for (unsigned int j = 0; j < 4; j++)
        {
            m_SphereShader->SetUniform3f(
                "lightPositions[" + std::to_string(j) + "]",
                m_LightPos[j].x,
                m_LightPos[j].y,
                m_LightPos[j].z
            );
            m_SphereShader->SetUniform3f(
                "lightColors[" + std::to_string(j) + "]",
                m_LightColor[j].x,
                m_LightColor[j].y,
                m_LightColor[j].z
            );
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_TexAlbedo);
        m_SphereShader->SetUniform1i("albedo", 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_TexNormal);
        m_SphereShader->SetUniform1i("normalMap", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_TexRoughness);
        m_SphereShader->SetUniform1i("roughness", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_TexMetallic);
        m_SphereShader->SetUniform1i("metallic", 3);

        glm::vec3 camPos = m_Camera->GetCameraLocation();
        m_SphereShader->SetUniform3f("cameraPos", camPos.x, camPos.y, camPos.z);
        m_Spheres[i].DrawObject();
    }

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    m_QuadShader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    m_QuadShader->SetUniform1i("screenTexture", 0);
    m_QuadShader->SetUniform1i("u_HDR", bHDR ? 1 : 0);
    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void test::TestTexturePBR::OnUpdate(GLFWwindow* window, float deltaTime)
{
    Test::OnUpdate(window, deltaTime);
    m_Camera->ProcessInput(window, deltaTime);
    bool hdrKeyPressing = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (hdrKeyPressing && !bHDRKeyPressed)
    {
        bHDR = !bHDR;
    }
    bHDRKeyPressed = hdrKeyPressing;

}

void test::TestTexturePBR::OnImGuiRender()
{
    Test::OnImGuiRender();
    ImGui::SliderFloat("Spacing", &m_SphereSpacing, 2.5f, 10.f);
    ImGui::SliderFloat3("Position", &m_SphereTranslation.x, -20.f, 20.f);
    std::string str = bHDR ? "HDR: ON" : "HDR OFF";
    ImGui::Text(str.c_str());
}
