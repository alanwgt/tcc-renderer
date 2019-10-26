#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include "core.h"

namespace wega
{
    Shader::~Shader()
    {
        if (!m_compiled)
            return;

        glDetachShader(m_program, m_fragment);
        glDetachShader(m_program, m_vertex);

        glDeleteShader(m_fragment);
        glDeleteShader(m_vertex);

        glDeleteProgram(m_program);
    }

    void Shader::ParseFiles()
    {
        const auto read_file = [&](std::string& f) -> std::string
        {
            auto p = Core::GetPath(f);
            std::ifstream ifs(p);

            if (!ifs)
                return {};

            std::stringstream buffer;
            buffer << ifs.rdbuf();
            ifs.close();

            return buffer.str();
        };

        m_vertex_raw = read_file(m_vertex_path);
        m_fragment_raw = read_file(m_fragment_path);

        if (m_vertex_raw.empty())
            std::cout << "ERROR::SHADER::VERTEX::PARSING_FAILED\n" << m_vertex_path << "\n";

        if (m_fragment_raw.empty())
            std::cout << "ERROR::SHADER::FRAGMENT::PARSING_FAILED\n" << m_fragment_path << "\n";
    }


    GLuint Shader::Compile(const char* shader_src, GLuint type)
    {
        // create a shader object
        GLuint id = glCreateShader(type);
        // attach the shader source code to the shader object and compile it

        glShaderSource(id, 1, &shader_src, nullptr);
        glCompileShader(id);

        int success;

        // check if the compilation was successful
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char info[512];
            glGetShaderInfoLog(id, 512, nullptr, info);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << info << "\n";
            return 0;
        }

        std::cout << "SHADER COMPILED!\n";
        return id;
    }

    bool Shader::Compile()
    {
        const char* c_vert_str = m_vertex_raw.c_str();
        const char* c_frag_str = m_fragment_raw.c_str();

        m_vertex = Compile(c_vert_str, GL_VERTEX_SHADER);
        m_fragment = Compile(c_frag_str, GL_FRAGMENT_SHADER);

        if (!m_vertex || ! m_fragment)
            return false;

        m_compiled = true;
        return true;
    }

    bool Shader::Link()
    {
        if (!m_compiled)
        {
            std::cout << "Shader must be compiled before linking happens!\n";
            return false;
        }

        // a program will be the one responsible for linking the shaders and executing the code
        m_program = glCreateProgram();
        glAttachShader(m_program, m_vertex);
        glAttachShader(m_program, m_fragment);
        glLinkProgram(m_program);
        glValidateProgram(m_program);

        int success;
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);

        if (!success)
        {
            char info[512];
            glGetShaderInfoLog(m_program, 512, nullptr, info);
            std::cout << "ERROR::SHADER::LINKING_ERROR\n" << info << "\n";
            return false;
        }

        m_linked = true;
        return true;
    }

    void Shader::Bind()
    {
        if (!m_compiled || !m_linked)
        {
            std::cout << "ERROR: shader MUST be compiled and linked before usag!\n";
            return;
        }

        glUseProgram(m_program);
    }

    void Shader::SetBool(const std::string& name, bool val) const
    {
        glUniform1i(glGetUniformLocation(m_program, name.c_str()), static_cast<int>(val));
    }

    void Shader::SetInt(const std::string& name, int val) const
    {
        glUniform1i(glGetUniformLocation(m_program, name.c_str()), val);
    }

    void Shader::SetFloat(const std::string& name, float val) const
    {
        glUniform1f(glGetUniformLocation(m_program, name.c_str()), val);
    }

    void Shader::Set3F(const std::string& name, float a, float b, float c) const
    {
        glUniform3f(glGetUniformLocation(m_program, name.c_str()), a, b, c);
    }

    void Shader::SetM4F(const std::string& name, glm::mat4 val) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
    }

    void Shader::SetV3(const std::string& name, glm::vec3 val) const
    {
        glUniform3f(glGetUniformLocation(m_program, name.c_str()), val.x, val.y, val.z);
    }
}
