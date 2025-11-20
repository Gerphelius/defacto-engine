#pragma once

#include <string>

#include <glad/glad.h>
#include "math/math.hpp"

namespace DF::Assets
{
    enum class ShaderType
    {
        PHONG,
        UNLIT,

        SHADERS_COUNT,
    };

    class Shader
    {
    public:
        explicit Shader(const std::string& vertexPath, const std::string& fragmentPath);

        ~Shader()
        {
            glDeleteProgram(m_id);
        }

        void use() const { glUseProgram(m_id); }

        void setUniform(const std::string& name, bool value) const;
        void setUniform(const std::string& name, int value) const;
        void setUniform(const std::string& name, float value) const;
        void setUniform(const std::string& name, Math::vec3 value) const;
        void setUniform(const std::string& name, Math::mat4 value) const;

    private:
        GLuint m_id{};

        GLuint compileShader(const std::string& path, GLenum type) const;
    };
}

