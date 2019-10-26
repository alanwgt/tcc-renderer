#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <cmath>

#include "raw_model.h"
#include "textured_model.h"
#include "entity.h"
#include "shader.h"
#include "my_math.h"

namespace wega
{
    class Renderer
    {
        static constexpr float FOV        = 70.0f;
        static constexpr float NEAR_PLANE = 0.1f;
        static constexpr float FAR_PLANE  = 1000.0f;

        glm::mat4 m_projection_matrix;

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
    public:
        Renderer(int width, int height)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            CreateProjectionMatrix(width, height);
        }

        void Prepare(void)
        {
            glEnable(GL_DEPTH_TEST);
            glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Render(RawModel* model)
        {
            glBindVertexArray(model->GetVAOId());
            glEnableVertexAttribArray(0);
            glDrawElements(GL_TRIANGLES, model->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glDisableVertexAttribArray(0);
            glBindVertexArray(0);
        }

        void Render(TexturedModel* textured_model)
        {
            RawModel* model = textured_model->GetRawModel();
            glBindVertexArray(model->GetVAOId());
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textured_model->GetTexture()->GetTextureID());
            glDrawElements(GL_TRIANGLES, model->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        void Render(Entity* entity, Shader* shader, const std::string& uniform_name)
        {
            TexturedModel* model = entity->GetModel();
            RawModel* raw = model->GetRawModel();
            glBindVertexArray(raw->GetVAOId());
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glm::mat4 transformation_matrix = CreateTransformationMatrix(
                    entity->GetPosition(),
                    entity->GetRotX(),
                    entity->GetRotY(),
                    entity->GetRotZ(),
                    entity->GetScale()
            );

            shader->SetM4F(uniform_name, transformation_matrix);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model->GetTexture()->GetTextureID());
            glDrawElements(GL_TRIANGLES, raw->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        inline glm::mat4 GetProjectionMatrix(void) const { return m_projection_matrix; }
    };
}
