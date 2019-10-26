#pragma once

#include <glad/glad.h>

namespace wega
{
    class RawModel
    {
        GLuint m_vao_id;
        GLuint m_vertex_count;
    public:
        RawModel(GLuint vao_id, GLuint vertex_count)
            : m_vao_id{vao_id}, m_vertex_count{vertex_count}
        {}

        inline GLuint GetVAOId() const { return m_vao_id; }
        inline GLuint GetVertexCount() const { return m_vertex_count; }
    };
}
