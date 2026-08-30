#pragma once
#include <functional>
#include <glew.h>
#include <string>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>

#include "glm/vec3.hpp"

namespace pbr
{
	struct PBRParams
	{
		glm::vec3 albedo;
		float roughness;
		float metallic;
	};
}

namespace test
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test();

		virtual void OnUpdate(GLFWwindow* window, float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPointer);

		virtual void OnUpdate(GLFWwindow* window, float deltaTime) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "Registering test" << name << std::endl;
			m_Tests.push_back(std::make_pair(name, [](){ return new T; })); // defer the creation of the test
		}

	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
	};
}
