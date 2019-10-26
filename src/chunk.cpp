#include "chunk.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace wega
{
void Chunk::GenerateIndices(void)
{
    unsigned int p = 0;

    for (unsigned int gz = 0; gz < m_sz - 1; gz++)
        for (unsigned int gx = 0; gx < m_sz - 1; gx++)
    {
        unsigned int tl = (gz * m_sz) + gx;
        unsigned int tr = tl + 1;
        unsigned int bl = ((gz + 1) * m_sz) + gx;
        unsigned int br = bl + 1;
        m_indices[p++] = tl;
        m_indices[p++] = bl;
        m_indices[p++] = tr;
        m_indices[p++] = tr;
        m_indices[p++] = bl;
        m_indices[p++] = br;
    }
}

void Chunk::GenerateMesh(void)
{
    unsigned int p = 0;

    for (unsigned int i = 0; i < m_sz; i++) 
        for (unsigned int j = 0; j < m_sz; j++)
    {
        GLdouble height = GetHeight(j, i);
        m_vertices[p * 3] = (double)j/((double)m_sz - 1) * TERRAIN_SIZE;
        m_vertices[p * 3 + 1] = height;
        m_vertices[p * 3 + 2] = (double)i/((double)m_sz - 1) * TERRAIN_SIZE;

        glm::vec3 normal = CalculateNormal(j, i);
        m_normals[p * 3] = normal.x;
        m_normals[p * 3 + 1] = normal.y;
        m_normals[p * 3 + 2] = normal.z;
        p++;
    }
}

glm::vec3 Chunk::CalculateNormal(unsigned int x, unsigned int z)
{
    double h_l = GetHeight(x - 1, z);
    double h_r = GetHeight(x + 1, z);
    double h_d = GetHeight(x, z - 1);
    double h_u = GetHeight(x, z + 1);
    glm::vec3 normal = glm::vec3{h_l - h_r, 2.0f, h_d - h_u};

    return glm::normalize(normal);
}

GLdouble Chunk::GetSteepness(unsigned int x, unsigned int z)
{
    GLdouble height = GetHeight(x, z);
    unsigned int nx = x + 1;
    unsigned int nz = z + 1;

    if (nx > m_sz - 1)
        nx = x - 1;
    if (nz > m_sz - 1)
        nz = z - 1;

    GLdouble dx = GetHeight(nx, z) - height;
    GLdouble dz = GetHeight(x, nz) - height;

    return glm::length(glm::vec2{dx, dz});
}

inline GLdouble Chunk::GetHeight(unsigned int x, unsigned int z)
{
    if (x < 0 || x >= m_sz || z < 0 || z >= m_sz)
        return 0;
    return *(m_height_map + x * m_sz + z);
}

void Chunk::SetHeight(unsigned int x, unsigned int z, double val)
{
	if (val > m_max_value)
		m_max_value = val;
	else if (val < m_min_value)
		m_min_value = val;
	
    *(m_height_map + x * m_sz + z) = val;
}
}
