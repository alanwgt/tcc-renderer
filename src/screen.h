#pragma once
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <sstream>

#include "my_math.h"
#include "chunk.h"
#include "stb_image_write.h"

namespace wega
{
class Screen
{
public:
	static std::ostringstream GenRandomName()
	{
		long int t = std::time(nullptr);
		std::ostringstream ss;
		ss << "print_" << t;
		return ss;
	}

	static std::string RandomFileName(const std::string& type)
	{
		return GenRandomName().str() + "." + type;
	}
	
    static std::string CaptureFromOpenGL(int width, int height)
    {
	    auto *buffer = new unsigned char[width * height * 3];
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	    const auto ss = GenRandomName();
	    const auto name = "opengl_prints/" + ss.str() + ".png";

	    auto *last_row = static_cast<unsigned char*>(buffer) + (width * 3 * (height - 1));

        if (!stbi_write_png(name.c_str(), width, height, 3, last_row, -3 * width))
            std::cerr << "Could not write PNG '" << name << "' to file!\n";
        else
            std::cout << "Screenshot saved to: " << name << ".\n";

        delete[] buffer;

        return "opengl_prints/" + ss.str();
    }

    static void HeightMapToPNG(Chunk* chunk, std::string name)
    {
        name += "_hm.png";
        unsigned int sz = chunk->GetSize();
        unsigned char* buffer = new unsigned char[sz * sz * 3];
        unsigned char* last_row = (unsigned char*)buffer + (sz * 3 * (sz - 1));
        unsigned int pos = 0;

        for (unsigned int i = 0; i < sz; i++)
            for (unsigned int j = 0; j < sz; j++)
        {
            auto t = chunk->GetHeight(j, i);
            auto res = Map(t, chunk->GetMinValue(), chunk->GetMaxValue(), 0, 255);
            buffer[pos++] = res;
            buffer[pos++] = res;
            buffer[pos++] = res;
        }

        if (!stbi_write_png(name.c_str(), sz, sz, 3, last_row, -3 * sz))
            std::cerr << "Could not write PNG '" << name << "' to file!\n";
        else
            std::cout << "Screenshot saved to: " << name << ".\n";

        delete[] buffer;
    }
};
}
