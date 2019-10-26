#pragma once

#include <glad/glad.h>
#include <memory>
#include <cstdlib>
#include <ctime>

#include "loader.h"
#include "raw_model.h"

namespace wega
{
    class Chunk
    {
        static constexpr float SIZE              = 800.0f;
        static constexpr int   VERTEX_COUNT      = 128;

        float m_x;
        float m_z;
        RawModel* m_raw_model;

    public:
        Chunk(int grid_x, int grid_z, Loader* loader)
            : m_x{grid_x * SIZE}, m_z{grid_z * SIZE}
        {
            m_raw_model = GenerateChunk(loader);
        }
        
        ~Chunk()
        {
            delete m_raw_model;
        }

        inline float GetX(void) const { return m_x; }
        inline float GetZ(void) const { return m_z; }
        inline RawModel* GetRawModel(void) { return m_raw_model; }

        static RawModel* GenerateChunk(Loader* loader)
        {
            const unsigned int count = VERTEX_COUNT * VERTEX_COUNT;
            float vertices[count * 3];
            float normals[count * 3];
            float texture_coords[count * 2];
            int indices[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];
            int vertex_pointer = 0;

            for (unsigned int i = 0; i < VERTEX_COUNT; i++)
            {
                for (unsigned int j = 0; j < VERTEX_COUNT; j++)
                {
                    vertices[vertex_pointer * 3] = (float)j/((float)VERTEX_COUNT - 1) * SIZE;
                    vertices[vertex_pointer * 3 + 1] = 0;
                    vertices[vertex_pointer * 3 + 2] = (float)i/((float)VERTEX_COUNT - 1) * SIZE;
                    normals[vertex_pointer * 3] = 0;
                    normals[vertex_pointer * 3 + 1] = 1;
                    normals[vertex_pointer * 3 + 2] = 0;
                    texture_coords[vertex_pointer * 2] = (float)j/((float)VERTEX_COUNT - 1);
                    texture_coords[vertex_pointer * 2 + 1] = (float)i/((float)VERTEX_COUNT - 1);
                    vertex_pointer++;
                }
            }

            int pointer = 0;

            for (unsigned int gz = 0; gz < VERTEX_COUNT - 1; gz++)
            {
                for (unsigned int gx = 0; gx < VERTEX_COUNT - 1; gx++)
                {
                    int top_left = (gz * VERTEX_COUNT) + gx;
                    int top_right = top_left + 1;
                    int bottom_left = ((gz + 1) * VERTEX_COUNT) + gx;
                    int bottom_right = bottom_left + 1;
                    indices[pointer++] = top_left;
                    indices[pointer++] = bottom_left;
                    indices[pointer++] = top_right;
                    indices[pointer++] = top_right;
                    indices[pointer++] = bottom_left;
                    indices[pointer++] = bottom_right;
                }
            }

            std::vector<GLfloat> v_vertices{vertices, vertices + count * 3};
            std::vector<GLfloat> v_normals{normals, normals + count * 3};
            std::vector<GLuint> v_indices{indices, indices + (6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1))};
            std::vector<GLfloat> v_tex_coords{texture_coords, texture_coords + count * 2};

            return loader->LoadToVAO(v_vertices, v_tex_coords, v_indices);
        }
    };

    class Terrain
    {
        static constexpr float TERRAIN_AMPLITUDE = 100.0f;

        std::unique_ptr<Chunk> m_chunks[9];
        int m_seed;
    public:
        Terrain()
        {
            std::srand(std::time(nullptr));
            // TODO: remove hardcoded values and terrain chunk generation
            m_chunks[4] = std::make_unique<Chunk>(256, 256);
            m_seed = std::rand();
        }

        ~Terrain()
        {
        }
    };

    class HeightGenerator
    {
    public:
        static float GenerateHeight(const int x, const int z)
        {
            return 1.0f;
        }
    };
}
