#pragma once

#include <glad/glad.h>
#include <vector>
#include <iostream>

#include "raw_model.h"

namespace wega
{
    class Loader
    {
        std::vector<GLuint> m_vaos;
        std::vector<GLuint> m_vbos;

        GLuint CreateVAO(void)
        {
            GLuint vao_id;
            glGenVertexArrays(1, &vao_id);
            m_vaos.push_back(vao_id);
            glBindVertexArray(vao_id);

            return vao_id;
        }

        void StoreDataInAttributeList(GLuint attribute_number, int attrib_size, std::vector<GLfloat>& data)
        {
            GLuint vbo_id;
            glGenBuffers(1, &vbo_id);
            m_vbos.push_back(vbo_id);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);
            // put the VBO into one of the VAO's attribute lists
            glVertexAttribPointer(attribute_number, attrib_size, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void UnbindVAO(void)
        {
            glBindVertexArray(0);
        }

        void BindIndicesBuffer(std::vector<GLuint>& indices)
        {
            GLuint ibo_id;
            glGenBuffers(1, &ibo_id);
            m_vbos.push_back(ibo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
        }

    public:
        Loader(){}

        ~Loader(void)
        {
            for (auto iter = m_vbos.begin(); iter != m_vbos.end(); iter++)
                glDeleteBuffers(1, &(*iter));
            for (auto iter = m_vaos.begin(); iter != m_vaos.end(); iter++)
                glDeleteVertexArrays(1, &(*iter));
        }

        RawModel* LoadToVAO(std::vector<GLfloat>& positions, std::vector<GLfloat>& texture_coords, std::vector<GLuint>& indices)
        {
            GLuint vao_id = CreateVAO();
            BindIndicesBuffer(indices);
            StoreDataInAttributeList(0, 3, positions);
            StoreDataInAttributeList(1, 2, texture_coords);
            UnbindVAO();

            return new RawModel(vao_id, indices.size()); 
        }

        RawModel* LoadToVAO(std::vector<GLfloat>& positions, std::vector<GLuint>& indices)
        {
            GLuint vao_id = CreateVAO();
            BindIndicesBuffer(indices);
            StoreDataInAttributeList(0, 3, positions);
            UnbindVAO();

            return new RawModel(vao_id, indices.size()); 
        }
    };
}
