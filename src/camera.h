#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

namespace wega
{
    class Camera
    {
        static constexpr float MOVE_AMOUNT = 0.2f;
        static constexpr float SPEED_MULTIPLIER = 10.0f;

        glm::vec3 m_position;
        float m_pitch, m_yaw, m_roll;
        GLFWwindow* m_window;
		float m_current_speed;
    public:
        Camera(GLFWwindow* window)
            : m_position{glm::vec3{0.0f}}, m_pitch{0.0f}, m_yaw{0.0f}, m_roll{0.0f}, m_window{window}, m_current_speed{0.0f}
        {}

        void Update()
        {
			m_current_speed = MOVE_AMOUNT;

            if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				m_current_speed *= SPEED_MULTIPLIER;
            if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
                m_position.z -= m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
                m_position.x += m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
                m_position.x -= m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
                m_position.z += m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
                m_position.y += m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
                m_position.y -= m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_J) == GLFW_PRESS)
                m_pitch += m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_K) == GLFW_PRESS)
                m_pitch -= m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
                m_yaw += m_current_speed;
            if (glfwGetKey(m_window, GLFW_KEY_H) == GLFW_PRESS)
                m_yaw -= m_current_speed;
        }

    	void UpdateByMouseDrag(const double x, const double y)
        {
        	if (abs(y) > 0.05)
				m_pitch += y * m_current_speed;
        	if (abs(x) > 0.05)
				m_yaw += x * m_current_speed;
        }

        inline glm::vec3 GetPosition(void) const { return m_position; }
        inline float GetPitch(void) const { return m_pitch; }
        inline float GetYaw(void) const { return m_yaw; }
        inline float GetRoll(void) const { return m_roll; }
    };
}
