#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

void Camera::Mouse_Callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		return;

	if (bFirstMove)
	{
		m_LastPosX = xpos;
		m_LastPosY = ypos;
		bFirstMove = false;
	}
	double offset_x = xpos - m_LastPosX;
	double offset_y = ypos - m_LastPosY;
	m_LastPosX = xpos;
	m_LastPosY = ypos;
	float sensitivity = 0.05f;

	offset_x *= sensitivity;
	offset_y *= sensitivity;

	m_Yaw += (float)offset_x;
	m_Pitch -= (float)offset_y;

	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;
}

Camera::Camera()
	: m_MoveSpeed(5.f), m_CameraLocation(0.f,0.f,0.f),
	m_LookAtDirection(0.f, 0.f, -1.f), m_UpDirection(0.f, 1.f, 0.f),
	m_Yaw(0), m_Pitch(0), m_TurnRate(30),
	m_LastPosY(540), m_LastPosX(960), m_FOV(45)
{
	
}

Camera::~Camera()
{
	// When the camera is destroyed (e.g. switching tests), detach the callback
	// and user pointer from the window and restore the cursor.
	if (m_Window)
	{
		glfwSetCursorPosCallback(m_Window, nullptr);
		glfwSetWindowUserPointer(m_Window, nullptr);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Camera::Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_FOV -= (float)yoffset;
	if (m_FOV < 1.0f)
		m_FOV = 1.0f;
	if (m_FOV > 45.f)
		m_FOV = 45.f;
}

glm::vec3 Camera::GetForwardDirection() const
{
	glm::vec3 direction(0,0,0);
	direction.x = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = -cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	return glm::normalize(direction);
}

void Camera::ProcessInput(GLFWwindow* window, float deltaTime)
{
	if (!window) return;

	if (!bInitialized)
	{
		bInitialized = true;
		m_Window = window;

		// Start in FPS mode: disable the cursor and register the mouse callback
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		bCursorDisabled = true;

		// Pass this to the callback through the window user pointer.
		// A captureless lambda can convert to a plain C function pointer.
		glfwSetWindowUserPointer(window, this);
		glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xpos, double ypos)
		{
			Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(w));
			if (camera)
				camera->Mouse_Callback(w, xpos, ypos);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset)
		{
			if (Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(w)))
				camera->Scroll_Callback(w,xoffset,yoffset);
		});
	}

	glm::vec3 forward = GetForwardDirection();
	if (glfwGetKey(window, GLFW_KEY_W))
		m_CameraLocation += forward * m_MoveSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S))
		m_CameraLocation -= forward * m_MoveSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A))
		m_CameraLocation += glm::normalize(glm::cross(m_UpDirection, forward)) * m_MoveSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D))
		m_CameraLocation -= glm::normalize(glm::cross(m_UpDirection, forward)) * m_MoveSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!bEscPressed)
		{
			bEscPressed = true;
			bCursorDisabled = !bCursorDisabled;
			glfwSetInputMode(window, GLFW_CURSOR, bCursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
			if (bCursorDisabled)
				bFirstMove = true; // reset mouse baseline to avoid a view jump
		}
	}
	else
	{
		bEscPressed = false;
	}
}

glm::mat4 Camera::GetLookAtMatrix() const
{
	return glm::lookAt(m_CameraLocation,
		m_CameraLocation + GetForwardDirection(),
		m_UpDirection
	);
}
