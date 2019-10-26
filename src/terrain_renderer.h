#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "terrain.h"
#include "shader.h"
#include "my_math.h"
#include "camera.h"

namespace wega
{
    class TerrainRenderer
    {
        static constexpr float FOV        = 70.0f;
        static constexpr float NEAR_PLANE = 0.1f;
        static constexpr float FAR_PLANE  = 1000.0f;

        glm::mat4 m_projection_matrix;
        Shader* m_shader;

        void CreateProjectionMatrix(int width, int height)
        {
            float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
            float y_scale = (1.0f / std::tan(glm::radians(FOV / 2.0f))) * aspect_ratio;
            float x_scale = y_scale / aspect_ratio;
            float frustrum_length = FAR_PLANE - NEAR_PLANE;

            m_projection_matrix = glm::mat4{0.0f};
            m_projection_matrix[0][0] = x_scale;
            m_projection_matrix[1][1] = y_scale;
            m_projection_matrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustrum_length);
            m_projection_matrix[2][3] = -1.0f;
            m_projection_matrix[3][2] = -((2 * NEAR_PLANE * FAR_PLANE) / frustrum_length);
            m_projection_matrix[3][3] = 0.0f;
        }

        void LoadTransformationMatrix(Chunk* c)
        {
            glm::mat4 transformation_matrix = CreateTransformationMatrix(glm::vec3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 1.0f);
            m_shader->SetM4F("transformation_matrix", transformation_matrix);
        }
    public:
        TerrainRenderer(Shader* shader, int width, int height)
            : m_shader{shader}
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            CreateProjectionMatrix(width, height);
        }

        void PrepareChunk(Chunk* c)
        {
            RawModel* model = c->GetRawModel();
            glBindVertexArray(model->GetVAOId());
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
        }

        void Unbind(void)
        {
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glBindVertexArray(0);
        }

        void Render(Camera* camera, std::vector<Chunk*>& chunks)
        {
            m_shader->Bind();
            m_shader->SetM4F("view_matrix", CreateViewMatrix(camera));
            for (auto i = chunks.begin(); i != chunks.end(); i++)
            {
                PrepareChunk(*i);
                LoadTransformationMatrix(*i);
                glDrawElements(GL_TRIANGLES, (*i)->GetRawModel()->GetVertexCount(), GL_UNSIGNED_INT, 0);
                Unbind();
            }
            glUseProgram(0);
        }

        inline glm::mat4 GetProjectionMatrix(void) const { return m_projection_matrix; }
    };
}
