
#include "TestShadowMapping.h"

#include "../../../Shader.h"
#include "../../../Texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include "ImGui/imgui.h"
#include "util/Camera.h"



void test::TestShadowMapping::OnRender()
{
    Test::OnRender();

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // ============================================================
    // PASS 1: Render the scene from the light's point of view
    // ============================================================

    m_shader->Bind();

    // Orthographic projection is suitable for this directional-light style shadow map
    glm::mat4 lightProj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 15.f);
    glm::mat4 lightView = glm::lookAt(m_lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 lightSpaceMatrix = lightProj * lightView;

    m_shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);


    // Render cubes into the depth map
    glBindVertexArray(m_cubeVAO);

    for (unsigned int i = 0; i < TOTAL_CUBE; i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.f), m_cubeModelMat[i]);

        // The rotation must be identical to the rotation used in pass 2
        float rotationAngle = 12.f + i * 10.f;
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.f, 1.f, 0.f));
        model = glm::translate(model, m_cubeTranslation[i]);
        m_shader->SetUniformMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    // Render plane into the depth map
    glBindVertexArray(m_planeVAO);

    glm::mat4 model = glm::mat4(1.f);
    m_shader->SetUniformMat4f("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // ============================================================
    // PASS 2: Render the scene from the camera's point of view
    // ============================================================

    glViewport(0, 0, 1920, 1080);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    /*
    // Debug depth map visualization
    glBindVertexArray(m_quadVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);

    m_quadShader->Bind();
    m_quadShader->SetUniform1i("depthMap", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    */


    m_objShader->Bind();

    // Texture unit 0: diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Texture unit 1: shadow depth map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);

    m_objShader->SetUniform1i("u_Texture", 0);
    m_objShader->SetUniform1i("u_DepthMap", 1);
    m_objShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
    m_objShader->SetUniform3f("u_LightPosition", m_lightPos.x, m_lightPos.y, m_lightPos.z);
    m_objShader->SetUniform3f("u_CameraLocation", m_camera->GetCameraLocation());


    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 500.f);
    glm::mat4 view = m_camera->GetLookAtMatrix();


    // Render cubes
    glBindVertexArray(m_cubeVAO);

    for (unsigned int i = 0; i < TOTAL_CUBE; i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.f), m_cubeModelMat[i]);

        // Apply the same rotation as in the depth pass
        float rotationAngle = 12.f + i * 10.f;
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.f, 1.f, 0.f));
        model = glm::translate(model, m_cubeTranslation[i]);
        glm::mat4 mvp = proj * view * model;

        m_objShader->SetUniformMat4f("u_Model", model);
        m_objShader->SetUniformMat4f("u_MVP", mvp);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    // Render plane
    glBindVertexArray(m_planeVAO);

    model = glm::mat4(1.f);
    glm::mat4 mvp = proj * view * model;

    m_objShader->SetUniformMat4f("u_Model", model);
    m_objShader->SetUniformMat4f("u_MVP", mvp);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void test::TestShadowMapping::OnUpdate(GLFWwindow* window, float deltaTime)
{
    Test::OnUpdate(window, deltaTime);
    m_camera->ProcessInput(window, deltaTime);
}


void test::TestShadowMapping::OnImGuiRender()
{
    Test::OnImGuiRender();
    ImGui::SliderFloat3("Cube Translation1", &m_cubeTranslation[0].x, -10.f, 10.f);
    ImGui::SliderFloat3("Cube Translation2", &m_cubeTranslation[1].x, -10.f, 10.f);
    ImGui::SliderFloat3("Cube Translation3", &m_cubeTranslation[2].x, -10.f, 10.f);
}


test::TestShadowMapping::TestShadowMapping()
{
    // ============================================================
    // Scene layout
    // ============================================================

    // Center cube: slightly elevated above the floor
    m_cubeModelMat.emplace_back(0.0f, 1.0f, 0.0f);
    m_cubeTranslation.emplace_back(0.f,0.f,0.f);

    // Right cube
    m_cubeModelMat.emplace_back(3.f, 0.0f, 1.5f);
    m_cubeTranslation.emplace_back(0.f, 0.f, 0.f);

    // Left cube
    m_cubeModelMat.emplace_back(-2.5f, 0.0f, 2.0f);
    m_cubeTranslation.emplace_back(0.f, 0.f, 0.f);



    // Load wood texture
    m_texture = loadTexture("res/textures/wood.png");


    // ============================================================
    // Cube vertex data
    // ============================================================

    float cubeVertices[] =
    {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

         // bottom face
         -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
          1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
          1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
          1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
         -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

         // top face
         -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
          1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
          1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
    };


    // Cube VAO / VBO
    glGenVertexArrays(1, &m_cubeVAO);
    glBindVertexArray(m_cubeVAO);

    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);


    // ============================================================
    // Debug quad
    // ============================================================

    float quadVertices[] =
    {
        // positions        // texture coordinates
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };


    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);

    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);


    // ============================================================
    // Plane
    // ============================================================

    float planeVertices[] =
    {
        // positions              // normals        // texture coordinates
         12.0f, -2.5f,  12.0f,    0.0f, 1.0f, 0.0f,  6.0f, 0.0f,
        -12.0f, -2.5f,  12.0f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -12.0f, -2.5f, -12.0f,    0.0f, 1.0f, 0.0f,  0.0f, 6.0f,

         12.0f, -2.5f,  12.0f,    0.0f, 1.0f, 0.0f,  6.0f, 0.0f,
        -12.0f, -2.5f, -12.0f,    0.0f, 1.0f, 0.0f,  0.0f, 6.0f,
         12.0f, -2.5f, -12.0f,    0.0f, 1.0f, 0.0f,  6.0f, 6.0f
    };


    glGenVertexArrays(1, &m_planeVAO);
    glBindVertexArray(m_planeVAO);

    glGenBuffers(1, &m_planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);


    // ============================================================
    // Camera and shaders
    // ============================================================

    m_camera = std::make_unique<Camera>();

    m_shader = std::make_unique<Shader>("res/shaders/DepthMap.shader");
    m_objShader = std::make_unique<Shader>("res/shaders/BlinnPhong_Shadow.shader");
    m_quadShader = std::make_unique<Shader>("res/shaders/debugDepth.shader");


    // ============================================================
    // Depth framebuffer
    // ============================================================

    glEnable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &m_fbo);
    glGenTextures(1, &m_depthMap);

    glBindTexture(GL_TEXTURE_2D, m_depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Use white outside the shadow map
    float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);

    // We only need the depth buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


test::TestShadowMapping::~TestShadowMapping()
{
}