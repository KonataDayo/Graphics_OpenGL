#pragma once
#include "glm/vec3.hpp"
#include "test/Test.h"

namespace test
{
	class TestHDR: public Test
	{
	private:

        // lighting info
        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;

	public:

		TestHDR();
		~TestHDR();

		void OnRender() override;
		void OnUpdate(GLFWwindow* window, float deltaTime) override;
		void OnImGuiRender() override;

	};
}
