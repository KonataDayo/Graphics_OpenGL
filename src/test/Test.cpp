#include "Test.h"
#include "ImGui/imgui.h"
#include "../Renderer.h"

namespace test
{
	Test::~Test()
	{
		GLCall(glClearColor(0.f, 0.f, 0.f, 1.0f)); 
	}

	TestMenu::TestMenu(Test*& currentTestPointer)
		: m_CurrentTest(currentTestPointer)
	{

	}

	void test::TestMenu::OnUpdate(GLFWwindow* window, float deltaTime)
	{
		Test::OnUpdate(window, deltaTime);
	}

	void test::TestMenu::OnRender()
	{
		Test::OnRender();
	}

	void test::TestMenu::OnImGuiRender()
	{
		Test::OnImGuiRender();
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
				m_CurrentTest = test.second();
		}

	}
	
}

