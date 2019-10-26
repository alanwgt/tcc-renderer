#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "core.h"

namespace fs = std::filesystem;

namespace wega
{
    enum class TextureType
    {
        RGB  = STBI_rgb,
        RGBA = STBI_rgb_alpha
    };

    class Texture
    {
        int m_width, m_height, m_channels;
        unsigned int m_pixel_count;
        bool m_loaded = false;
        unsigned char* m_img = nullptr;
        std::string m_file;
        GLuint m_texture_id;

        void LoadIntoOpenGL()
        {
            if (!m_loaded) return;

            GL_CHECK(glGenTextures(1, &m_texture_id));
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture_id));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            /* GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER)); */
            /* GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER)); */
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

            if (m_channels == 3)
                GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_img));
            else if (m_channels == 4)
                GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_img));

            Unbind();
        }

    public:
        Texture(const std::string& file, TextureType t)
            : m_file{file}
        {
            auto p = Core::GetPath(file);
            stbi_set_flip_vertically_on_load(1);
            std::string s = p.string();
            const char* absolute_path = s.c_str();

            m_img = stbi_load(
                    absolute_path,
                    &m_width,
                    &m_height,
                    &m_channels,
                    as_integer(t));

            if (!m_img) return;

            m_pixel_count = m_width * m_height;            
            m_loaded = true;
            LoadIntoOpenGL();
            stbi_image_free(m_img);
        }

        ~Texture(void)
        {
            if (!m_loaded) return;

            glDeleteTextures(1, &m_texture_id);
        }

        inline void Bind(void) const { GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_texture_id)); }
        inline void Unbind(void) const { GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); }

        inline GLuint GetTextureID(void) const { return m_texture_id; }
        inline unsigned int GetPixelCount(void) const { return m_pixel_count; }
        inline bool IsLoaded(void) const { return m_loaded; }
        inline int GetWidth(void) const { return m_width; }
        inline int GetHeight(void) const { return m_height; }
    };
}
