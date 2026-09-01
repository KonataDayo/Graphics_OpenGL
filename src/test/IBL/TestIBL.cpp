#include "TestIBL.h"
#include "../Object.h"
#include "../../../Shader.h"
#include "../../../Texture.h"
#include "ImGui/imgui.h"
#include "util/Camera.h"
#include "util/ObjParser.h"

void test::TestIBL::CaptureEnvMap()
{
    // HDR image skybox
    m_HDRI = loadHDRTexture("res/textures/christmas_photo_studio_01_4k.hdr");
    m_FromEquirectangleToCubemapShader = std::make_unique<Shader>("res/shaders/FromEquirectangleToCubemapShader.shader");
    m_skyboxShader = std::make_unique<Shader>("res/shaders/Skybox.shader");
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &m_skyboxVAO);
    glBindVertexArray(m_skyboxVAO);
    glGenBuffers(1, &m_skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glBindVertexArray(0);

    // capture skybox textures
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);
    glGenFramebuffers(1, &m_captureFBO);
    glGenRenderbuffers(1, &m_captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);

    glGenTextures(1, &m_capturedCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_capturedCubemap);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
       glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    m_FromEquirectangleToCubemapShader->Bind();
    m_FromEquirectangleToCubemapShader->SetUniformMat4f("proj", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_HDRI);
    m_FromEquirectangleToCubemapShader->SetUniform1i("envMap", 0);

    glBindVertexArray(m_skyboxVAO);
    for (unsigned int i = 0; i < 6; i++)
    {
        m_FromEquirectangleToCubemapShader->SetUniformMat4f("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_capturedCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_capturedCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

    glViewport(0, 0, 1920, 1080);

    // Generate Irradiance Map
    glGenTextures(1, &m_irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
    for (unsigned int i = 0; i < 6; i++)
    {
        // allocate
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB16F,
            IRRADIANCE_MAP_RESOLUTION,
            IRRADIANCE_MAP_RESOLUTION,
            0,
            GL_RGB,
            GL_FLOAT,
            nullptr
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_irradianceMapShader = std::make_unique<Shader>("res/shaders/DiffuseConvolution_IBL.shader");

    glViewport(0, 0, IRRADIANCE_MAP_RESOLUTION, IRRADIANCE_MAP_RESOLUTION);
    m_irradianceMapShader->Bind();
    m_irradianceMapShader->SetUniformMat4f("proj", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_capturedCubemap);
    m_irradianceMapShader->SetUniform1i("envMap", 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    glBindVertexArray(m_skyboxVAO);
    glDepthFunc(GL_LEQUAL);
    for (unsigned int i = 0; i < 6; i++)
    {
        m_irradianceMapShader->SetUniformMat4f("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glDepthFunc(GL_LESS);
    glViewport(0, 0, 1920, 1080);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

test::TestIBL::TestIBL()
{
    // Spheres
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    util::ObjParser::ParseOBJ("res/meshes/pbr-sphere.obj", vertices, indices);
    m_SphereMesh = std::make_shared<util::Mesh>(vertices, indices);
    for (unsigned int i = 0; i < m_SphereRow; i++)
    {
        for (unsigned int j = 0; j < m_SphereColumn; j++)
        {
            glm::vec3 position, rotation, scale;
            position = glm::vec3((float)(i * m_SphereSpacing), (float)(j * -m_SphereSpacing), 0.f);
            rotation = glm::vec3(0.f, 0.f, 0.f);
            scale = glm::vec3(1.f, 1.f, 1.f);
            m_Spheres.emplace_back(m_SphereMesh, position, rotation, scale);
        }
    }

    m_SphereShader = std::make_unique<Shader>("res/shaders/ThreeDimension.shader");
	m_Camera = std::make_unique<Camera>();
    glEnable(GL_DEPTH_TEST);

    // post-processing
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    glGenFramebuffers(1, &m_quadFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_quadFBO);
    glGenTextures(1, &m_quadTexture);
    glBindTexture(GL_TEXTURE_2D, m_quadTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1080, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_quadTexture, 0);
    glGenRenderbuffers(1, &m_quadRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_quadRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1920,1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_quadRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_quadShader = std::make_unique<Shader>("res/shaders/SimpleHDR.shader");

	CaptureEnvMap();

   
}

test::TestIBL::~TestIBL()
{
}

void test::TestIBL::OnRender()
{
	Test::OnRender();
    glBindFramebuffer(GL_FRAMEBUFFER, m_quadFBO);

    glClearColor(0.f, 1.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 1000.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 cameraDir = m_Camera->GetLookAtMatrix();
    view = cameraDir * view;

    glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(1.f,0.f,1.f));
    m_SphereShader->Bind();
    glm::mat4 mvp = proj * view * model;
    m_SphereShader->SetUniformMat4f("u_MVP",mvp);
    m_Spheres[0].DrawObject();
    
    glDepthFunc(GL_LEQUAL);

    m_skyboxShader->Bind();
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glm::mat4 skyboxMVP = proj * skyboxView;
    m_skyboxShader->SetUniformMat4f("u_mvp",skyboxMVP);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_capturedCubemap);
    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
    m_skyboxShader->SetUniform1i("skybox", 1);

    glBindVertexArray(m_skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);

    // PASS 2
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_quadShader->Bind();
    m_quadShader->SetUniform1i("b_HDR",1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_quadTexture);
    m_quadShader->SetUniform1i("screenTexture", 0);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void test::TestIBL::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
    m_Camera->ProcessInput(window, deltaTime);
}

void test::TestIBL::OnImGuiRender()
{
	Test::OnImGuiRender();
    ImGui::SliderFloat3("Position", &m_SphereTranslation.x, -20.f, 20.f);
}
