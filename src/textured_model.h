#pragma once

#include "raw_model.h"
#include "model_texture.h"

namespace wega
{
    class TexturedModel
    {
        RawModel* m_raw_model;
        ModelTexture* m_texture;
    public:
        TexturedModel(RawModel* model, ModelTexture* texture)
            : m_raw_model{model}, m_texture{texture}
        {}
        
        inline RawModel* GetRawModel(void) const { return m_raw_model; }
        inline ModelTexture* GetTexture(void) const { return m_texture; }
    };
}
