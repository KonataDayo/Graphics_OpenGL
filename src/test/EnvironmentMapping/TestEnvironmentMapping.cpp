#include "TestEnvironmentMapping.h"

#include "../../../Shader.h"
#include "../../../Texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include "util/Camera.h"

test::TestEnvironmentMapping::TestEnvironmentMapping()
	: m_Reflectance(1.f), m_RefractiveIndex(1.0f / 1.5f)
{
	m_TexturesFaces = {
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};

    float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

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

    // Cube
    glGenVertexArrays(1, &m_CubeVAO);
    glBindVertexArray(m_CubeVAO);
    glGenBuffers(1, &m_CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0); // positions
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float))); // normals

    // Skybox
    glGenVertexArrays(1, &m_SkyboxVAO);
    glBindVertexArray(m_SkyboxVAO);
    glGenBuffers(1, &m_SkyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0); // positions

    m_CubeShader = std::make_unique<Shader>("res/shaders/Cubemap.shader");
    m_SkyboxShader = std::make_unique<Shader>("res/shaders/Skybox.shader");
    m_Camera = std::make_unique<Camera>();

    m_SkyboxTexture = loadCubemap(m_TexturesFaces);
}

test::TestEnvironmentMapping::~TestEnvironmentMapping()
{
}

void test::TestEnvironmentMapping::OnRender()
{
	Test::OnRender();
    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f/1080.f, 0.1f, 1000.f);
	glm::mat4 cameraDirection = m_Camera->GetLookAtMatrix();
    glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
    view = cameraDirection * view;

    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // Reflective Cube
    glm::mat4 modelCube_Reflect = glm::translate(glm::mat4(1.f), glm::vec3(-2.f, 0.f, -1.f));
    glBindVertexArray(m_CubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxTexture);
    m_CubeShader->Bind();
    m_CubeShader->SetUniformMat4f("u_proj",proj);
    m_CubeShader->SetUniformMat4f("u_view",view);
    m_CubeShader->SetUniformMat4f("u_model",modelCube_Reflect);
    glm::vec3 cameraPos = m_Camera->GetCameraLocation();
    m_CubeShader->SetUniform3f("u_cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Skybox
    glDepthFunc(GL_LEQUAL);
	glBindVertexArray(m_SkyboxVAO);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxTexture);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(cameraDirection));

    glm::mat4 skyboxMVP = proj * skyboxView;
    m_SkyboxShader->Bind();
    m_SkyboxShader->SetUniformMat4f("u_mvp",skyboxMVP);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void test::TestEnvironmentMapping::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
    m_Camera->ProcessInput(window, deltaTime);
}

void test::TestEnvironmentMapping::OnImGuiRender()
{
	Test::OnImGuiRender();

}
