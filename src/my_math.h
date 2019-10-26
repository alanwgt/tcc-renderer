#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

namespace wega
{
    glm::mat4 CreateTransformationMatrix(glm::vec3 translation, float rx, float ry, float rz, float scale)
    {
        glm::mat4 matrix{1.0f};
        matrix = glm::scale(glm::vec3{scale, scale, scale});
        matrix = glm::rotate(matrix, glm::radians(rx), glm::vec3{1.0f, 0.0f, 0.0f});
        matrix = glm::rotate(matrix, glm::radians(ry), glm::vec3{0.0f, 1.0f, 0.0f});
        matrix = glm::rotate(matrix, glm::radians(rz), glm::vec3{0.0f, 0.0f, 1.0f});
        matrix = glm::translate(matrix, translation);

        return matrix;
    }

    glm::mat4 CreateViewMatrix(Camera* c)
    {
        glm::mat4 view_matrix{1.0f};
        view_matrix = glm::rotate(view_matrix, glm::radians(c->GetPitch()), glm::vec3{1.0f, 0.0f, 0.0f});
        view_matrix = glm::rotate(view_matrix, glm::radians(c->GetYaw()), glm::vec3{0.0f, 1.0f, 0.0f});
        glm::vec3 camera_position = c->GetPosition();
        // roll all the world against the camera position
        glm::vec3 neg_cp = glm::vec3{-camera_position.x, -camera_position.y, -camera_position.z};
        view_matrix = glm::translate(view_matrix, neg_cp);

        return view_matrix;
    }

    static glm::mat4 GenerateProjectionMatrix(int width, int height, float fov, float near_plane, float far_plane)
    {
        float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        float y_scale = (1.0f / std::tan(glm::radians(fov / 2.0f))) * aspect_ratio;
        float x_scale = y_scale / aspect_ratio;
        float frustrum_length = far_plane - near_plane;

        glm::mat4 projection_matrix = glm::mat4{0.0f};
        projection_matrix[0][0] = x_scale;
        projection_matrix[1][1] = y_scale;
        projection_matrix[2][2] = -((far_plane + near_plane) / frustrum_length);
        projection_matrix[2][3] = -1.0f;
        projection_matrix[3][2] = -((2 * near_plane * far_plane) / frustrum_length);
        projection_matrix[3][3] = 0.0f;

        return projection_matrix;
    }


    float Map(float value, float original_min, float original_max, float target_min, float target_max)
    {
        return (value - original_min) * (target_max - target_min) / (original_max - original_min) + target_min;
    }
}
