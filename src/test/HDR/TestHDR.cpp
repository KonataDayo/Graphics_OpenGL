#include "TestHDR.h"
#include <iostream>
#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../../../Shader.h"
#include "../../../IndexBuffer.h"
#include "../../../VertexArray.h"
#include "../../../VertexBuffer.h"
#include "../../../VertexBufferLayout.h"
#include "ImGui/imgui.h"
#include "util/Camera.h"

test::TestHDR::TestHDR()
{
    // =========================================================
    // 1. LIGHTS
    // =========================================================
    lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f));
    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
    lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(glm::vec3(0.3f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));


    // =========================================================
    // 2. TUNNEL VERTICES
    // =========================================================

    float vertices[] =
    {
        // =====================================================
        // Back face
        // =====================================================

        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,

         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

        // =====================================================
        // Front face
        // =====================================================

        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,

         1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

        // =====================================================
        // Left face
        // =====================================================

        -1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,-1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,-1.0f,-1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

        -1.0f, -1.0f,-1.0f,-1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,-1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 1.0f,-1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

        // =====================================================
        // Right face
        // =====================================================

         1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         1.0f, -1.0f,-1.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         1.0f,  1.0f,-1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

         1.0f, -1.0f,-1.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

         // =====================================================
         // Bottom face
         // =====================================================

         -1.0f, -1.0f,-1.0f, 0.0f,-1.0f, 0.0f,  0.0f, 1.0f,
          1.0f, -1.0f,-1.0f, 0.0f,-1.0f, 0.0f,  1.0f, 1.0f,
          1.0f, -1.0f, 1.0f, 0.0f,-1.0f, 0.0f,  1.0f, 0.0f,

          1.0f, -1.0f, 1.0f, 0.0f,-1.0f, 0.0f,  1.0f, 0.0f,
         -1.0f, -1.0f, 1.0f, 0.0f,-1.0f, 0.0f,  0.0f, 0.0f,
         -1.0f, -1.0f,-1.0f, 0.0f,-1.0f, 0.0f,  0.0f, 1.0f,

         // =====================================================
         // Top face
         // =====================================================

         -1.0f, 1.0f,-1.0f, 0.0f,1.0f,0.0f,  0.0f,1.0f,
          1.0f, 1.0f, 1.0f, 0.0f,1.0f,0.0f,  1.0f,0.0f,
          1.0f, 1.0f,-1.0f, 0.0f,1.0f,0.0f,  1.0f,1.0f,

          1.0f, 1.0f, 1.0f, 0.0f,1.0f,0.0f,  1.0f,0.0f,
         -1.0f, 1.0f,-1.0f, 0.0f,1.0f,0.0f,  0.0f,1.0f,
         -1.0f, 1.0f, 1.0f, 0.0f,1.0f,0.0f,  0.0f,0.0f
    };


    unsigned int indices[] =
    {
        0, 1, 2,
        3, 4, 5,

        6, 7, 8,
        9, 10, 11,

        12, 13, 14,
        15, 16, 17,

        18, 19, 20,
        21, 22, 23,

        24, 25, 26,
        27, 28, 29,

        30, 31, 32,
        33, 34, 35
    };


    // =========================================================
    // 3. SIMPLE LIGHT CUBE
    //
    // Only position is required.
    // =========================================================

    float lightCubeVertices[] =
    {
        // back
        -1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,

         1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

        // front
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,

         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,

        // left
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        // right
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,

         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,

         // bottom
         -1.0f,-1.0f,-1.0f,
          1.0f,-1.0f,-1.0f,
          1.0f,-1.0f, 1.0f,

          1.0f,-1.0f, 1.0f,
         -1.0f,-1.0f, 1.0f,
         -1.0f,-1.0f,-1.0f,

         // top
         -1.0f,1.0f,-1.0f,
          1.0f,1.0f, 1.0f,
          1.0f,1.0f,-1.0f,

          1.0f,1.0f,1.0f,
         -1.0f,1.0f,-1.0f,
         -1.0f,1.0f,1.0f
    };


    unsigned int lightIndices[] =
    {
        0, 1, 2,
        3, 4, 5,

        6, 7, 8,
        9, 10, 11,

        12, 13, 14,
        15, 16, 17,

        18, 19, 20,
        21, 22, 23,

        24, 25, 26,
        27, 28, 29,

        30, 31, 32,
        33, 34, 35
    };


    // =========================================================
    // 4. FULLSCREEN QUAD
    // =========================================================

    float quadVertices[] =
    {
        // position            // UV

        -1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,    1.0f, 0.0f
    };


    unsigned int quadIndices[] =
    {
        0, 1, 2,
        1, 2, 3
    };


    // =========================================================
    // 5. OPENGL STATE
    // =========================================================
    glEnable(GL_DEPTH_TEST);
    // Camera is inside the cube.
    glDisable(GL_CULL_FACE);

    // =========================================================
    // 6. TUNNEL VAO
    // =========================================================
    m_VAO = std::make_unique<VertexArray>();
    m_VAO->Bind();

    m_VBO = std::make_unique<VertexBuffer>(vertices,sizeof(vertices));
    m_IBO = std::make_unique<IndexBuffer>(indices,36);
    m_Shader = std::make_unique<Shader>("res/shaders/Tunnel.shader");
    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // UV
    m_VBO->Bind();
    m_IBO->Bind();
    m_VAO->AddBuffer(*m_VBO,layout);


    // =========================================================
    // 7. TUNNEL TEXTURE
    // =========================================================

    glGenTextures(1,&m_TunnelTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_TunnelTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    unsigned char* data = stbi_load(
        "res/textures/wood.png",
        &m_Width,
        &m_Height,
        &m_Channels,
        0
    );

    if (!data)
    {
        std::cout << "Failed to load wood.png\n";
    }
    else
    {
        GLenum format;

        if (m_Channels == 1)
            format = GL_RED;
        else if (m_Channels == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            m_Width,
            m_Height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    // =========================================================
    // 8. HDR FRAMEBUFFER
    // =========================================================

    glGenFramebuffers(1,&m_HDRFBO);

    glBindFramebuffer(GL_FRAMEBUFFER,m_HDRFBO);


    // -------------------------
    // HDR color buffer
    // -------------------------

    glGenTextures(1,&m_HDRColorBuffer);

    glBindTexture(GL_TEXTURE_2D,m_HDRColorBuffer);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA16F,
        1920,
        1080,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_HDRColorBuffer,
        0
    );


    // -------------------------
    // Depth buffer
    // -------------------------

    glGenRenderbuffers(1,&m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER,m_RBO);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT24,
        1920,
        1080
    );


    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        m_RBO
    );


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<< "HDR framebuffer is NOT complete!\n";
    }
    else
    {
        std::cout<< "HDR framebuffer is complete!\n";
    }


    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // =========================================================
    // 9. LIGHT VISUALIZATION VAO
    // =========================================================

    m_LightVAO = std::make_unique<VertexArray>();
    m_LightVAO->Bind();
    m_LightVBO = std::make_unique<VertexBuffer>(
        lightCubeVertices,
        sizeof(lightCubeVertices)
    );

    m_LightIBO = std::make_unique<IndexBuffer>(
        lightIndices,
        36
    );

    m_LightShader = std::make_unique<Shader>(
        "res/shaders/LightSource.shader"
    );


    VertexBufferLayout lightLayout;

    lightLayout.Push<float>(3);


    m_LightVBO->Bind();
    m_LightIBO->Bind();

    m_LightVAO->AddBuffer(
        *m_LightVBO,
        lightLayout
    );


    // =========================================================
    // 10. FULLSCREEN QUAD VAO
    // =========================================================

    m_QuadVAO = std::make_unique<VertexArray>();
    m_QuadVAO->Bind();

    m_QuadVBO = std::make_unique<VertexBuffer>(
        quadVertices,
        sizeof(quadVertices)
    );

    m_QuadIBO = std::make_unique<IndexBuffer>(
        quadIndices,
        6
    );

    m_QuadShader = std::make_unique<Shader>(
        "res/shaders/HDR.shader"
    );


    VertexBufferLayout quadLayout;

    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);


    m_QuadVBO->Bind();
    m_QuadIBO->Bind();

    m_QuadVAO->AddBuffer(
        *m_QuadVBO,
        quadLayout
    );


    // =========================================================
    // 11. CAMERA
    // =========================================================

    m_Camera = std::make_unique<Camera>();
}


test::TestHDR::~TestHDR()
{
    if (m_TunnelTexture)
    {
    	glDeleteTextures(1,&m_TunnelTexture);
    }


    if (m_HDRColorBuffer)
    {
        glDeleteTextures(1,&m_HDRColorBuffer);
    }


    if (m_HDRFBO)
    {
        glDeleteFramebuffers(1,&m_HDRFBO);
    }


    if (m_RBO)
    {
        glDeleteRenderbuffers(1,&m_RBO);
    }
}


void test::TestHDR::OnRender()
{
    Test::OnRender();


    // =========================================================
    // MATRICES
    // =========================================================

    glm::mat4 proj = glm::perspective(glm::radians(45.0f),1920.0f / 1080.0f,0.1f,1000.0f);
    glm::mat4 lookAtMatrix = m_Camera->GetLookAtMatrix();
    glm::mat4 view = lookAtMatrix *glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-3.0f));

    // =========================================================
    // TUNNEL MODEL
    // =========================================================

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(0.0f,0.0f,0.0f));
    model =glm::scale(model,glm::vec3(5.0f,5.0f,50.0f));
    glm::mat4 mvp =proj * view * model;


    // =========================================================
    // PASS 1
    //
    // Tunnel + light markers -> HDR FBO
    // =========================================================


    glBindFramebuffer(GL_FRAMEBUFFER,m_HDRFBO);
    glViewport(0,0,1920,1080);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // =========================================================
    // Tunnel
    // =========================================================

    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP",mvp);
    m_Shader->SetUniformMat4f("u_Model",model);

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_TunnelTexture);
    m_Shader->SetUniform1i("u_Texture",0);

    // Inverse normals because camera is inside tunnel
    m_Shader->SetUniform1i("u_InverseNormals",1);

    // Lights
    for (unsigned int i = 0;i < lightPositions.size();++i)
    {
        std::string positionName ="u_LightPositions[" +std::to_string(i) +"]";
        std::string colorName ="u_LightColors[" +std::to_string(i) +"]";

        m_Shader->SetUniform3f(positionName,lightPositions[i].x,lightPositions[i].y,lightPositions[i].z);
        m_Shader->SetUniform3f(colorName,lightColors[i].x,lightColors[i].y,lightColors[i].z);
    }
    Renderer renderer;
    renderer.Draw(*m_VAO,*m_IBO,*m_Shader);


    // =========================================================
    // Light visualization cubes
    // =========================================================
    /*
    m_LightShader->Bind();

    for (unsigned int i = 0;i < lightPositions.size();++i)
    {
        glm::mat4 lightModel =glm::mat4(1.0f);
        lightModel = glm::translate(lightModel,lightPositions[i]);
        // Make them small
        lightModel = glm::scale(lightModel,glm::vec3(0.15f));
        glm::mat4 lightMVP =proj *view *lightModel;

        m_LightShader->SetUniformMat4f("u_MVP",lightMVP);
        m_LightShader->SetUniform3f("u_LightColor",lightColors[i].x,lightColors[i].y,lightColors[i].z);
        renderer.Draw(*m_LightVAO,*m_LightIBO,*m_LightShader);
    }
	*/

    // =========================================================
    // PASS 2
    //
    // HDR Color Buffer -> Screen
    // =========================================================

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	m_QuadShader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_HDRColorBuffer);
    m_QuadShader->SetUniform1i("u_HDRBuffer",0);
    m_QuadShader->SetUniform1f("u_Exposure",m_Exposure);
    Renderer quadRenderer;
    quadRenderer.Draw(*m_QuadVAO,*m_QuadIBO,*m_QuadShader);
	
}


void test::TestHDR::OnUpdate(GLFWwindow* window,float deltaTime
)
{
    Test::OnUpdate(window,deltaTime);


    // =========================================================
    // Camera
    // =========================================================
	m_Camera->ProcessInput(window,deltaTime);


    // =========================================================
    // Exposure
    //
    // Q -> decrease
    // E -> increase
    // =========================================================

    const float exposureSpeed = 1.5f;


    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        m_Exposure -= exposureSpeed *deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        m_Exposure += exposureSpeed *deltaTime;
    }

    // Clamp
    m_Exposure = glm::clamp(m_Exposure,0.05f,10.0f);
}


void test::TestHDR::OnImGuiRender()
{
    Test::OnImGuiRender();
    ImGui::Text("Exposure: %.2f",m_Exposure);
    ImGui::Text("Q: decrease exposure");
    ImGui::Text("E: increase exposure");
}