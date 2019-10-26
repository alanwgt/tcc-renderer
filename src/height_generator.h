#pragma once

#include <glm/glm.hpp>
#include <glm/geometric.hpp>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <functional>
#include <random>
#include <cmath>

#include <noise/noise.h>
#include <noise/module/perlin.h>

#include "noiseutils.h"
#include "perlin_noise.h"
#include "chunk.h"
#include "screen.h"

namespace wega
{
    class HeightGenerator
    {
    public:
        static constexpr double AMPLITUDE        = 100.0;
        static constexpr double FREQUENCY        = 8.0;
        static constexpr int OCTAVES             = 4.0;
    private:
        int m_seed;
        Chunk* m_chunk;
        siv::PerlinNoise* m_perlin_noise;
        unsigned int m_terrain_size;
        std::mt19937 m_generator;
        double m_min = 999999.9, m_max = -999999.9;
        double m_threshold = -1.0;

        double GetNoise(int x, int z)
        {
            return m_perlin_noise->octaveNoise(x / FREQUENCY, z / FREQUENCY, OCTAVES);
        }

        void ApplyLayer(std::function<double (int, int, int)>& f)
        {
            for (unsigned int i = 0; i < m_terrain_size; i++)
                for (unsigned int j = 0; j < m_terrain_size; j++)
                {
                    double current_value = m_chunk->GetHeight(j, i);
                    double new_value = f(current_value, j, i);
                    SetHeight(j, i, new_value);
                    /* *(m_terrain_data + i * m_terrain_size + j) = new_value; */
                }
        }

        double Rand(int min, int max)
        {
            std::uniform_int_distribution<> dis{min, max};
            return dis(m_generator);
        }

        void SetHeight(unsigned int x, unsigned int z, double val)
        {
            if (val < 0) val = 0;
            else if (val > AMPLITUDE) val = AMPLITUDE;

            if (val > m_max) m_max = val;
            else if (val < m_min) m_min = val;

            if (m_chunk->GetHeight(x, z) < m_threshold)
                return;

            m_chunk->SetHeight(x, z, val);
        }

    public:
        HeightGenerator(Chunk* chunk, unsigned int terrain_size)
            : m_chunk{chunk}, m_terrain_size{terrain_size}
        {
            std::random_device rd;
            m_generator = std::mt19937{rd()};

            std::srand(std::time(nullptr));
            m_seed = std::rand();
            m_perlin_noise = new siv::PerlinNoise{static_cast<uint32_t>(m_seed)};
        }

        ~HeightGenerator()
        {
            delete m_perlin_noise;
        }

        void SetThreshold(double val) { m_threshold = val; }
        void DisableThreshold(void) { m_threshold = -1.0; }
        double GetMinHeight(void) const { return m_min; }
        double GetMaxHeight(void) const { return m_max; }

        void PrintInfo(void)
        {
            std::cout << "MIN: " << m_min << "\nMAX: " << m_max << "\n";
        }

        double Get(int x, int z)
        {
            if (x < 0 || z < 0)
                return 0.0f;

            return GetNoise(x, z) * AMPLITUDE;
        }

        void AddVoronoiLayer(unsigned int peak_count, double dropoff, double falloff)
        {
            static double s_voronoi_max_height = AMPLITUDE;
            static double s_voronoi_min_height = AMPLITUDE / 2.0;
            /* static double s_voronoi_dropoff = 0.1f; */
            /* static double s_voronoi_falloff = 5.0f; */
            static glm::vec2 s_zero = glm::vec2{0.0f};

            for (unsigned int i = 0; i < peak_count; i++)
            {
                int rx = Rand(0, m_terrain_size);
                int ry = Rand(0, m_terrain_size);
                double peak_height = Rand(s_voronoi_min_height, s_voronoi_max_height);
                double max_distance = glm::distance(s_zero, glm::vec2{m_terrain_size, m_terrain_size});
                glm::vec2 peak_location = glm::vec2{rx, ry};

                std::function<double (int, int, int)> f = [&](int c, int x, int z)
                {
                    if (x == rx && z == ry)
                        return peak_height;

                    double distance_to_peak = glm::distance(peak_location, glm::vec2{x, z})/* / max_distance*/;
                    double h = peak_height - distance_to_peak * falloff - std::pow(distance_to_peak, dropoff);
                    
                    return c < h ? h : c;
                };

                ApplyLayer(f);
            }
        }

    	void SetRandomValues()
        {
			std::function<double(int, int, int)> f = [&](int c, int x, int z)
			{
				return Rand((int)0, (int)AMPLITUDE);
			};

			ApplyLayer(f);
        }

        void AddPerlinNoiseLayer(unsigned int octaves, double persistance, double scale = 1.0f)
        {
            std::function<double (int, int, int)> f = [&](int c, int x, int z)
            {
                return c + fbm(x, z, octaves, persistance) * AMPLITUDE * scale;
            };

            ApplyLayer(f);
        }

        void AddPerlinNoiseLayer(unsigned int octaves, double persistance, int x_offset, int z_offset, bool additive = true)
        {
            std::function<double (int, int, int)> f = [&](int c, int x, int z)
            {
                double t = fbm(x + x_offset, z + z_offset, octaves, persistance) * AMPLITUDE;
                if (additive)
                    return c + t;
                return t;
            };

            ApplyLayer(f);
        }

    	void DiamondSquare()
        {
			const auto size = m_chunk->GetSize();

        	// initialization
			SetHeight(0, 0, Rand(0, AMPLITUDE));
			SetHeight(size - 1, 0, Rand(0, AMPLITUDE));
			SetHeight(0, size - 1, Rand(0, AMPLITUDE));
			SetHeight(size - 1, size - 1, Rand(0, AMPLITUDE));

        	// square step
			SetHeight(
				static_cast<int>((size - 1) / 2.0),
				static_cast<int>((size - 1) / 2.0),
				(m_chunk->GetHeight(0, 0) +
					m_chunk->GetHeight(size - 1, 0) +
					m_chunk->GetHeight(0, size - 1) +
					m_chunk->GetHeight(size - 1, size - 1)) / 4.0 + Rand(0, AMPLITUDE));
        }
    	
    	void ApplyHeightMap(utils::NoiseMap& hm)
        {
			utils::RendererImage renderer;
			utils::Image image;
			renderer.SetSourceNoiseMap(hm);
			renderer.SetDestImage(image);
			renderer.Render();
			utils::WriterBMP writer;
			writer.SetSourceImage(image);
			writer.SetDestFilename("heightmaps/" + Screen::RandomFileName("bmp"));
        	writer.WriteDestFile();
        	
        	for (auto x = 0; x < m_terrain_size; x++)
        		for (auto z = 0; z < m_terrain_size; z++)
        	{
					auto nsz = hm.GetValue(x, z);
					nsz *= AMPLITUDE;

					m_chunk->SetHeight(x, z, nsz);
        	}
        }

        void AddMidPointDisplacement(Chunk* chunk, double roughness, double dampener)
        {
            unsigned int width = chunk->GetSize() - 1;
            // quão grande é o quadrado atual
            unsigned int square_size = width;
            /* double height = AMPLITUDE; */
            double height_dampener = std::pow(dampener, -1 * roughness);

            double max_height = AMPLITUDE;
            double min_height = 0;

            unsigned int corner_x, corner_y, mid_x, mid_y, pmid_xl, pmid_xr,
                         pmid_yu, pmid_yd;

            while (square_size > 0)
            {
                // diamond step
                for (unsigned int y = 0; y < width; y += square_size)
                    for (unsigned int x = 0; x < width; x += square_size)
                {
                    corner_x = x + square_size;
                    corner_y = y + square_size;
                    mid_x = static_cast<unsigned int>(x + square_size / 2.0);
                    mid_y = static_cast<unsigned int>(y + square_size / 2.0);

                    // tamanho setado pela média dos cantos
                    SetHeight(mid_x, mid_y, (
                                chunk->GetHeight(x, y) + 
                                chunk->GetHeight(corner_x, y) + 
                                chunk->GetHeight(x, corner_y) + 
                                chunk->GetHeight(corner_x, corner_y)) / 4.0 +
                                Rand(min_height, max_height));
                }
            	
                // square step
                for (unsigned int y = 0; y < width; y += square_size)
                    for (unsigned int x = 0; x < width; x += square_size)
                {
                    corner_x = x + square_size;
                    corner_y = y + square_size;
                    mid_x = static_cast<unsigned int>(x + square_size / 2.0);
                    mid_y = static_cast<unsigned int>(y + square_size / 2.0);
                    
                    pmid_xr = static_cast<unsigned int>(mid_x + square_size);
                    pmid_yu = static_cast<unsigned int>(mid_y + square_size);
                    pmid_xl = static_cast<unsigned int>(mid_x - square_size);
                    pmid_yd = static_cast<unsigned int>(mid_y - square_size);

                    /*if (pmid_xl <= 0 || pmid_yd <= 0 || pmid_xr >= width - 1 || pmid_yu >= width - 1)
                        continue;*/

                    SetHeight(mid_x, y, 
                            (chunk->GetHeight(mid_x, mid_y) +
                             chunk->GetHeight(x, y) +
                             chunk->GetHeight(mid_x, pmid_yd) +
                             chunk->GetHeight(corner_x, y)) / 4.0 +
                            Rand(min_height, max_height));

                    SetHeight(mid_x, corner_y,
                            (chunk->GetHeight(x, corner_y) +
                             chunk->GetHeight(mid_x, mid_y) +
                             chunk->GetHeight(corner_x, corner_y) +
                             chunk->GetHeight(mid_x, pmid_yu)) / 4.0 +
                            Rand(min_height, max_height));

                    SetHeight(x, mid_y,
                            (chunk->GetHeight(x, y) +
                                chunk->GetHeight(pmid_xl, mid_y) +
                                chunk->GetHeight(x, corner_y) +
                                chunk->GetHeight(mid_x, mid_y)) / 4.0 +
                            Rand(min_height, max_height));

                    SetHeight(corner_x, mid_y,
                            (chunk->GetHeight(mid_x, y) +
                                chunk->GetHeight(mid_x, mid_y) +
                                chunk->GetHeight(corner_x, corner_y) +
                                chunk->GetHeight(pmid_xr, mid_y)) / 4.0 +
                            Rand(min_height, max_height));
                }

                // 1 / 4 dos quadrados cada iteração do loop
                square_size = static_cast<unsigned int>(square_size / 2.0);
                max_height *= height_dampener;
                min_height *= height_dampener;
            }
        }

        // fractal brownian motion
		[[nodiscard]] double fbm(const int x, const int z, const unsigned int octaves, const double persistance) const
        {
            double total = 0.0f;
            double frequency = 1.0f;
            double amplitude = 1.0f;
            double max_value = 0.0f;

            for (unsigned int i = 0; i < octaves; i++)
            {
                total += m_perlin_noise->noise0_1(x / frequency, z / frequency) * amplitude;
                max_value += amplitude;

                amplitude *= persistance;
                frequency *= 2;
            }

            return total / max_value;
        }
    };
}
