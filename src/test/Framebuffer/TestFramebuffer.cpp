#include "TestFramebuffer.h"
#include "../../../Texture.h"
#include "util/Camera.h"
#include "util/Mesh.h"
#include "../../../Shader.h"
#include "glm/gtc/matrix_transform.hpp"

test::TestFramebuffer::TestFramebuffer()
{

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_CubeVAO);
    glGenBuffers(1, &m_CubeVBO);
    glBindVertexArray(m_CubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));

    glGenVertexArrays(1, &m_PlaneVAO);
    glGenBuffers(1, &m_PlaneVBO);
    glBindVertexArray(m_PlaneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PlaneVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(float)));

    // Texture
    m_CubeTexture = loadTexture("res/textures/wood.png");
    m_PlaneTexture = loadTexture("res/textures/Marble05_rgh.jpg");

    // Framebuffer
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    // create color attachment
    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    // allocate space
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    // attach
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);

    glGenRenderbuffers(1, &m_TextureRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureRenderBuffer);
    // allocate
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    // attach
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureRenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
	    std::cout << "Framebuffer incomplete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_CubeShader = std::make_unique<Shader>("res/shaders/TextureOnly.shader");
    m_PlaneShader = std::make_unique<Shader>("res/shaders/TextureOnly.shader");
    m_QuadShader = std::make_unique<Shader>("res/shaders/Postprocessing.shader");

    m_Camera = std::make_unique<Camera>();
}

test::TestFramebuffer::~TestFramebuffer()
{
}

void test::TestFramebuffer::OnRender()
{
	Test::OnRender();
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Pass 1
    // Cube
    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f/1080.f, 0.1f, 1000.f);
    glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,-3.f));
    glm::mat4 cameraDirection = m_Camera->GetLookAtMatrix();
    view = cameraDirection * view;
    glm::mat4 cubeModel1 = glm::translate(glm::mat4(1.f), glm::vec3(-1.0f, 0.0f, -1.0f));
	glm::mat4 cubeMVP1 = proj * view * cubeModel1;
    glBindVertexArray(m_CubeVAO);
	m_CubeShader->Bind();
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_CubeTexture);
    m_CubeShader->SetUniform1i("u_Texture",0);
    m_CubeShader->SetUniformMat4f("u_MVP",cubeMVP1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 cubeModel2 = glm::translate(glm::mat4(1.f), glm::vec3(2.0f, 0.0f, 0.0f));
    glm::mat4 cubeMVP2 = proj * view * cubeModel2;
    m_CubeShader->SetUniformMat4f("u_MVP", cubeMVP2);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 planeModel = glm::scale(glm::mat4(1.f), glm::vec3(1.f,1.f, 1.f));
    glm::mat4 planeMVP = proj * view * planeModel;
    glBindVertexArray(m_PlaneVAO);
    glBindTexture(GL_TEXTURE_2D, m_PlaneTexture);
    m_PlaneShader->SetUniformMat4f("u_MVP",planeMVP);
    m_PlaneShader->SetUniform1i("u_Texture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Pass 2
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_QuadShader->Bind();
    glBindVertexArray(m_QuadVAO);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    m_QuadShader->SetUniform1i("screenTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void test::TestFramebuffer::OnUpdate(GLFWwindow* window, float deltaTime)
{
	Test::OnUpdate(window, deltaTime);
	m_Camera->ProcessInput(window,deltaTime);
}

void test::TestFramebuffer::OnImGuiRender()
{
	Test::OnImGuiRender();
}
