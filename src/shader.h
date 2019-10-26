#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace wega
{
    class Shader
    {
        GLuint m_vertex, m_fragment;
        GLuint m_program;
        bool m_compiled = false, m_linked = false;
        std::string m_vertex_path, m_fragment_path;
        std::string m_vertex_raw, m_fragment_raw;

        GLuint Compile(const char* shader_src, GLuint type);
    public:
        Shader(const std::string& vertex, const std::string& fragment)
            : m_vertex_path(vertex), m_fragment_path(fragment)
        {
            ParseFiles();
            if (m_vertex_raw.empty() || m_fragment_raw.empty())
                return;
            if (Compile())
                Link();
        }

        ~Shader();

        inline GLuint const vertex() const { return m_vertex; }
        inline GLuint const fragment() const { return m_fragment; }

        bool Compile();
        bool Link();
        void ParseFiles();
        void Bind();

        void SetBool(const std::string& name, bool val) const;
        void SetInt(const std::string& name, int val) const;
        void SetFloat(const std::string& name, float val) const;
        void Set3F(const std::string& name, float a, float b, float c) const;
        void SetM4F(const std::string& name, glm::mat4 val) const;
        void SetV3(const std::string& name, glm::vec3 val) const;
    };
}
