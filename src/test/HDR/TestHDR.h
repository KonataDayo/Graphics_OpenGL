#pragma once

#include <memory>
#include <vector>

#include "glm/vec3.hpp"
#include "test/Test.h"

class VertexBuffer;
class Camera;
class IndexBuffer;
class VertexArray;
class Shader;

namespace test
{
    class TestHDR : public Test
    {
    private:

        // =========================================================
        // Lighting
        // =========================================================

        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;


        // =========================================================
        // Tunnel
        // =========================================================

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<Shader> m_Shader;

        unsigned int m_TunnelTexture = 0;

        int m_Width = 0;
        int m_Height = 0;
        int m_Channels = 0;


        // =========================================================
        // Light visualization cubes
        // =========================================================

        std::unique_ptr<VertexArray> m_LightVAO;
        std::unique_ptr<IndexBuffer> m_LightIBO;
        std::unique_ptr<VertexBuffer> m_LightVBO;
        std::unique_ptr<Shader> m_LightShader;


        // =========================================================
        // HDR Framebuffer
        // =========================================================

        unsigned int m_HDRFBO = 0;
        unsigned int m_HDRColorBuffer = 0;
        unsigned int m_RBO = 0;


        // =========================================================
        // Fullscreen Quad
        // =========================================================

        std::unique_ptr<VertexArray> m_QuadVAO;
        std::unique_ptr<IndexBuffer> m_QuadIBO;
        std::unique_ptr<VertexBuffer> m_QuadVBO;
        std::unique_ptr<Shader> m_QuadShader;


        // =========================================================
        // Camera
        // =========================================================

        std::unique_ptr<Camera> m_Camera;


        // =========================================================
        // HDR settings
        // =========================================================

        float m_Exposure = 1.0f;


    public:

        TestHDR();
        ~TestHDR();

        void OnRender() override;
        void OnUpdate(GLFWwindow* window, float deltaTime) override;
        void OnImGuiRender() override;
    };
}