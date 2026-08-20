#include <fstream>
#include <glew.h>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <sstream>
#include "Renderer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw_gl3.h"
#include "test/TestCamera.h"
#include "test/TestClearColor.h"
#include "test/TestCube3D.h"
#include "test/TestFOV.h"
#include "test/TestTexture2D.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) std::cout<< "Error!" << std::endl;

    std::cout<< glGetString(GL_VERSION) << std::endl;

    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    glm::vec3 translationA(50, 50, 0);
	glm::vec3 translationB(200, 50, 0);

    // test
    test::Test* currentTest = nullptr;

    test::TestMenu* testMenu = new test::TestMenu(currentTest);
    testMenu->RegisterTest<test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<test::TestTexture2D>("Texture 2D");
    testMenu->RegisterTest<test::TestCube3D>("Draw A Cube");
    testMenu->RegisterTest<test::TestFOV>("FOV Test");
    testMenu->RegisterTest<test::TestCamera>("Camera Test");

    currentTest = testMenu;

    float currentFrame = 0.f;
    float lastFrame = 0.f;

    while (!glfwWindowShouldClose(window))
    {
        renderer.Clear();

    	ImGui_ImplGlfwGL3_NewFrame();
        if (currentTest)
        {
            currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

	        currentTest->OnUpdate(window ,deltaTime);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-"))
            {
	            delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete currentTest;
    if (currentTest != testMenu)
		delete testMenu;

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
