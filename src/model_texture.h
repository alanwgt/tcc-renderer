#pragma once

#include <glad/glad.h>

#include "texture.h"

namespace wega
{
    class ModelTexture
    {
        GLuint m_texture_id;
    public:
        ModelTexture(GLuint texture_id)
            : m_texture_id{texture_id}
        {
            
        }

        inline GLuint GetTextureID(void) const { return m_texture_id; }
    };
}
