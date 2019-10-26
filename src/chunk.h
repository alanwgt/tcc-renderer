#pragma once

#include <cstring>
#include <limits>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace wega
{
class Chunk
{
    static constexpr double TERRAIN_SIZE = 800.0;
    unsigned int m_sz, m_sz_squared;
    unsigned int m_grid_x, m_grid_y;
	double m_min_value, m_max_value;
    GLdouble* m_vertices;
    GLdouble* m_normals;
    GLdouble* m_height_map;
    GLuint* m_indices;
    int m_indices_size, m_vertices_size, m_normals_size, m_height_map_size;

    glm::vec3 CalculateNormal(unsigned int x, unsigned int z);
    void GenerateIndices(void);
public:
    Chunk(unsigned int sz, unsigned int x, unsigned int y)
        : m_sz(sz), m_grid_x(x), m_grid_y(y)
    {
		m_sz_squared = sz * sz;

        m_vertices_size = m_sz_squared * 3;
        m_normals_size = m_sz_squared * 3;
        m_height_map_size = m_sz_squared;
        m_indices_size = 6 * (m_sz - 1) * (m_sz - 1);

        m_vertices = new GLdouble[m_vertices_size];
        m_normals = new GLdouble[m_normals_size];
        m_height_map = new GLdouble[m_height_map_size];
        m_indices = new GLuint[m_indices_size];
		m_min_value = std::numeric_limits<double>::max();
		m_max_value = std::numeric_limits<double>::min();

        // preenche o heightmap com zeros
        std::memset(m_height_map, 0, sizeof(GLdouble) * m_height_map_size);
        GenerateMesh();
        GenerateIndices();
    }

    ~Chunk()
    {
        delete[] m_vertices;
        delete[] m_normals;
        delete[] m_indices;
        delete[] m_height_map;
    }

    void GenerateMesh(void);
    GLdouble GetSteepness(unsigned int x, unsigned int z);
    inline GLdouble GetHeight(unsigned int x, unsigned int z);
    void SetHeight(unsigned int x, unsigned int z, double val);

    GLdouble* GetVertices(void) const { return m_vertices; }
    GLdouble* GetNormals(void) const { return m_normals; }
    GLdouble* GetHeightMap(void) const { return m_height_map; }
    GLuint* GetIndices(void) const { return m_indices; }
    int GetIndicesSize(void) const { return m_indices_size; }
    int GetVerticesSize(void) const { return m_vertices_size; }
    int GetNormalsSize(void) const { return m_normals_size; }
    int GetHeightMapSize(void) const { return m_height_map_size; }
    unsigned int GetSize(void) const { return m_sz; }
	double GetMinValue(void) const { return m_min_value; }
	double GetMaxValue(void) const { return m_max_value; }
};
}
