#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <functional>

namespace fs = std::filesystem;

#define log(x) { std::cout << x << std::endl; }
#define panic(x) { std::cout << x << std::endl; return -1; }

namespace wega
{
    #define GL_CHECK(x) do          \
    {                               \
        x;                          \
        GLenum err = glGetError();  \
        if (err != GL_NO_ERROR) { std::cout << err << "\n"; } \
    } while (0)

    template <typename Enumeration>
    auto as_integer(Enumeration const value)
        -> typename std::underlying_type<Enumeration>::type
    {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    static const char* SRC_FULL_PATH = "W:\\dev\\cpp\\tcc\\src";

    class Core
    {
    public:
        static void DebugOpenGL();
        static void ClearColor(float r, float g, float b, float a);
        static void DrawRainbowTriangle(GLFWwindow* window);

        inline static fs::path GetPath(std::string& relative) { return fs::path(SRC_FULL_PATH) / relative; }
        inline static fs::path GetPath(const std::string& relative) { return fs::path(SRC_FULL_PATH) / relative; }
        inline static fs::path GetPath(const char* relative) { return fs::path(SRC_FULL_PATH) / relative; }

        static bool IterateFile(std::string& f, std::function<void (std::string&)> cb)
        {
            auto path = GetPath(f);
            std::ifstream in{path};

            if (!in) return false;

            std::string str;

            while (std::getline(in, str))
                cb(str);

            in.close();
            return true;
        }
    private:
        Core() {}
    };
}
