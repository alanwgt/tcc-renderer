#pragma once

#include <glm/glm.hpp>

#include "textured_model.h"

namespace wega
{
    class Entity
    {
        TexturedModel* m_textured_model;
        glm::vec3 m_position;
        float m_rx, m_ry, m_rz;
        float m_scale;

    public:
        Entity(TexturedModel* model, glm::vec3 pos, float rx, float ry, float rz, float scale)
            : m_textured_model{model}, m_position{pos}, m_rx{rx}, m_ry{ry}, m_rz{rz}, m_scale{scale}
        {}

        void Translate(float dx, float dy, float dz)
        {
            m_position.x += dx;
            m_position.y += dy;
            m_position.z += dz;
        }

        void Rotate(float dx, float dy, float dz)
        {
            m_rx += dx;
            m_ry += dy;
            m_rz += dz;
        }

        inline TexturedModel* GetModel(void) const { return m_textured_model; }
        inline glm::vec3 GetPosition(void) const { return m_position; }
        inline float GetRotX(void) const { return m_rx; }
        inline float GetRotY(void) const { return m_ry; }
        inline float GetRotZ(void) const { return m_rz; }
        inline float GetScale(void) const { return m_scale; }
    };
}
