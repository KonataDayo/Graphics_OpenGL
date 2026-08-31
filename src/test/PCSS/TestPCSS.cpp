#include "TestPCSS.h"

#include <random>

#include "../../../Shader.h"
#include "../../../Texture.h"
#include "glm/gtc/matrix_transform.hpp"
#include "ImGui/imgui.h"
#include "util/Camera.h"


std::vector<glm::vec2> test::TestPCSS::PoissonDiskSampling()
{
	float radius = 1.f / sqrt((float)POISSON_MAX_SAMPLES);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> r_dis(radius, 2 * radius);
    std::uniform_real_distribution<float> a_dis(0.f, 2.f * (float)PI);

    std::vector<glm::vec2> result;
    result.emplace_back(0.f,0.f);
    std::vector<glm::vec2> activeList;
    activeList.emplace_back(0.f, 0.f);
    // default starting point: (0,0)
    int lastIndex = -1;
    while (!activeList.empty() && result.size() < POISSON_MAX_SAMPLES)
    {
		std::uniform_int_distribution<int> index_dis(0, activeList.size()-1);
        int randomIndex = index_dis(gen);
	    glm::vec2 p = activeList[randomIndex];
        bool bFoundValid = false;
    	for (int attempt = 0; attempt < POISSON_MAX_INVALID_POINT_NUM; attempt++)
        {
			bool bValid = true;
			float r = r_dis(gen);
		    float angle = a_dis(gen) ;
			glm::vec2 samplePoint = p + glm::vec2(r * cos(angle), r * sin(angle));
	        for (const glm::vec2& existingPoint : result)
	        {
		        if (glm::distance(existingPoint, samplePoint) < radius)
		        {
					bValid = false; // invalid
                    break;
		        }
	        }
	        if (bValid)
	        {
				activeList.push_back(samplePoint);
    			result.push_back(samplePoint);
                bFoundValid = true;
                break;
	        }
        }
        if (!bFoundValid)
			activeList.erase(activeList.begin() + randomIndex);
    }

    float maxRadius = 0.0f;
    glm::vec2 center(0.0f); 
    for (const auto& p : result) 
    {
        center += p;
    }
    if (!result.empty()) center /= static_cast<float>(result.size());

    for (auto& p : result) 
    {
        p -= center; 
        maxRadius = std::max(maxRadius, glm::length(p));
    }

    if (maxRadius > 0.0f) 
    {
        for (auto& p : result) 
        {
            p /= maxRadius;
        }
    }

    while (result.size() < POISSON_MAX_SAMPLES) 
    {
        result.emplace_back(0.0f, 0.0f);
    }
    return result;
}

void test::TestPCSS::OnRender()
{
    Test::OnRender();

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // ============================================================
    // PASS 1: Render the scene from the light's point of view
    // ============================================================

    m_shader->Bind();

    // Orthographic projection is suitable for this directional-light style shadow map
    glm::mat4 lightProj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 20.f);
    glm::mat4 lightView = glm::lookAt(m_lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 lightSpaceMatrix = lightProj * lightView;

    m_shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, DEPTH_MAP_WIDTH_PCF, DEPTH_MAP_HEIGHT_PCF);
    glClear(GL_DEPTH_BUFFER_BIT);


    // Render cubes into the depth map
    glBindVertexArray(m_cubeVAO);

    for (unsigned int i = 0; i < TOTAL_CUBE; i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.f), m_cubeModelMat[i]);

        // The rotation must be identical to the rotation used in pass 2
        model = glm::translate(model, m_cubeTranslation[i]);
        model = glm::rotate(model,glm::radians(m_rot.x), glm::vec3(1.f,0.f,0.f));
        model = glm::rotate(model, glm::radians(m_rot.y), glm::vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, glm::radians(m_rot.z), glm::vec3(0.f, 0.f, 1.f));
        model = glm::scale(model, glm::vec3(5.0f, 1.f,1.f));
    	m_shader->SetUniformMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    // Render plane into the depth map
    glBindVertexArray(m_planeVAO);

    glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(5.f,1.f,5.f));
    m_shader->SetUniformMat4f("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // ============================================================
    // PASS 2: Render the scene from the camera's point of view
    // ============================================================

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (bDebugDepthMode)
    {
        glViewport(0, 0, DEPTH_MAP_WIDTH_PCF, DEPTH_MAP_HEIGHT_PCF);
	    // Debug depth map visualization
	    glBindVertexArray(m_quadVAO);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, m_depthMap);

	    m_quadShader->Bind();
	    m_quadShader->SetUniform1i("depthMap", 0);

	    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    else
    {
	    glViewport(0, 0, 1920, 1080);
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
        m_objShader->SetUniform1i("dm_width", DEPTH_MAP_WIDTH_PCF);
        m_objShader->SetUniform1i("dm_height", DEPTH_MAP_HEIGHT_PCF);
        m_objShader->SetUniform1i("filter_size", m_filterSize);
        m_objShader->SetUniformVec2f("poisson_disk", POISSON_MAX_SAMPLES, m_poissonDisk);
        

	    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 500.f);
	    glm::mat4 view = m_camera->GetLookAtMatrix();


	    // Render cubes
	    glBindVertexArray(m_cubeVAO);

	    for (unsigned int i = 0; i < TOTAL_CUBE; i++)
	    {
	        glm::mat4 model = glm::translate(glm::mat4(1.f), m_cubeModelMat[i]);

	        // Apply the same rotation as in the depth pass
	        model = glm::translate(model, m_cubeTranslation[i]);
            model = glm::rotate(model, glm::radians(m_rot.x), glm::vec3(1.f, 0.f, 0.f));
            model = glm::rotate(model, glm::radians(m_rot.y), glm::vec3(0.f, 1.f, 0.f));
            model = glm::rotate(model, glm::radians(m_rot.z), glm::vec3(0.f, 0.f, 1.f));
            model = glm::scale(model, glm::vec3(5.0f, 1.f, 1.f));
	    	glm::mat4 mvp = proj * view * model;

	        m_objShader->SetUniformMat4f("u_Model", model);
	        m_objShader->SetUniformMat4f("u_MVP", mvp);

	        glDrawArrays(GL_TRIANGLES, 0, 36);
	    }


	    // Render plane
	    glBindVertexArray(m_planeVAO);

	    model = glm::scale(glm::mat4(1.f), glm::vec3(5.f, 1.f, 5.f));
	    glm::mat4 mvp = proj * view * model;

	    m_objShader->SetUniformMat4f("u_Model", model);
	    m_objShader->SetUniformMat4f("u_MVP", mvp);

	    glDrawArrays(GL_TRIANGLES, 0, 6);

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}


void test::TestPCSS::OnUpdate(GLFWwindow* window, float deltaTime)
{
    Test::OnUpdate(window, deltaTime);
    m_camera->ProcessInput(window, deltaTime);
    bool bDebugModeKeyPressing = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (bDebugModeKeyPressing && !bDebugKeyPressed)
    {
	    bDebugDepthMode = !bDebugDepthMode;
    }
    bDebugKeyPressed = bDebugModeKeyPressing;
}


void test::TestPCSS::OnImGuiRender()
{
    Test::OnImGuiRender();
    ImGui::SliderFloat3("Cube Translation1", &m_cubeTranslation[0].x, -10.f, 10.f);
    ImGui::SliderFloat3("Cube Translation2", &m_cubeTranslation[1].x, -10.f, 10.f);
    ImGui::SliderFloat3("Cube Translation3", &m_cubeTranslation[2].x, -10.f, 10.f);
	ImGui::SliderInt("Filter Size", &m_filterSize, 1, 7);
    ImGui::SliderFloat3("Rotation", &m_rot.x ,0.f, 180.f);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
}


test::TestPCSS::TestPCSS()
{
    // ============================================================
    // Scene layout
    // ============================================================

	// Light
    m_lightPos = glm::vec3(3.0f, 7.0f, 3.0f);

    // Cube 1：主要遮挡物
    m_cubeModelMat.emplace_back(0.0f, -0.5f, 0.0f);
    m_cubeTranslation.emplace_back(0.0f, 0.0f, 0.0f);

    // Cube 2：放远一点，用来观察第二个阴影
    m_cubeModelMat.emplace_back(3.5f, -1.5f, 1.5f);
    m_cubeTranslation.emplace_back(0.0f, 0.0f, 0.0f);

    // Cube 3
    m_cubeModelMat.emplace_back(-3.5f, -1.5f, 1.0f);
    m_cubeTranslation.emplace_back(0.0f, 0.0f, 0.0f);



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
    m_objShader = std::make_unique<Shader>("res/shaders/BlinnPhong_PCF.shader");
	m_quadShader = std::make_unique<Shader>("res/shaders/debugDepth.shader");


    // ============================================================
    // Depth framebuffer
    // ============================================================

    glEnable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &m_fbo);
    glGenTextures(1, &m_depthMap);

    glBindTexture(GL_TEXTURE_2D, m_depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, DEPTH_MAP_WIDTH_PCF, DEPTH_MAP_HEIGHT_PCF, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


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

	m_poissonDisk = PoissonDiskSampling();
    for (const glm::vec2& vec : m_poissonDisk)
    {
	    std::cout<<"x: "<<vec.x<<" y: "<<vec.y<<std::endl;
    }
}


test::TestPCSS::~TestPCSS()
{
}