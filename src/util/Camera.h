#pragma once
#include <GLFW/glfw3.h>
#include <glm/detail/type_mat.hpp>

#include "glm/vec3.hpp"

class Camera
{
private:
	// Move Around
	glm::vec3 m_CameraLocation;
	glm::vec3 m_UpDirection;
	glm::vec3 m_LookAtDirection;
	float m_MoveSpeed;

	// Look Around
	float m_Yaw;
	float m_Pitch;
	float m_TurnRate; // degree per second
	bool bFirstMove = true;
	double m_LastPosX, m_LastPosY;
	void Mouse_Callback(GLFWwindow* window, double xpos, double ypos);

	// Input
	GLFWwindow* m_Window = nullptr;
	bool bInitialized = false;
	bool bCursorDisabled = false;
	bool bEscPressed = false;

	// Scroll
	float m_FOV;
	void Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset);

	glm::vec3 GetForwardDirection() const;
public:
	Camera();
	~Camera();

	void ProcessInput(GLFWwindow* window, float deltaTime);

	glm::mat4 GetLookAtMatrix() const;

	inline float GetFOV() const { return m_FOV; }
};
