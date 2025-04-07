#pragma once

#include <string>

#include <glad/glad.h>
#include "utils/math.hpp"

namespace DF::Render
{
    class ShaderProgram
    {
    private:
        GLuint m_id{};

    public:
        explicit ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

        ~ShaderProgram()
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
        GLuint compileShader(const std::string& path, GLenum type) const;
    };
}

