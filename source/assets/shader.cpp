#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "utils/file_io.hpp"

namespace DF::Assets
{
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
        : m_id{ glCreateProgram() }
    {
        GLuint vertexShader{ compileShader(vertexPath, GL_VERTEX_SHADER) };
        GLuint fragmentShader{ compileShader(fragmentPath, GL_FRAGMENT_SHADER) };

        glAttachShader(m_id, vertexShader);
        glAttachShader(m_id, fragmentShader);
        glLinkProgram(m_id);

        int success;
        char infoLog[512];
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(m_id, 512, NULL, infoLog);

            std::cout << "Shader program error: " << infoLog << '\n';
        }

        // Need to detach shader before deletion because the driver
        // won't free GPU memory until the shader program is deleted
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml
        glDetachShader(m_id, vertexShader);
        glDeleteShader(vertexShader);
        glDetachShader(m_id, fragmentShader);
        glDeleteShader(fragmentShader);
    }
 
    void Shader::setUniform(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::setUniform(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::setUniform(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::setUniform(const std::string& name, Math::vec3 value) const
    {
        glUniform3f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z);
    }

    void Shader::setUniform(const std::string& name, Math::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    GLuint Shader::compileShader(const std::string& path, GLenum type) const
    {
        GLuint shader{ glCreateShader(type) };
        auto shaderSource{ File::read(path) };
        auto source{ shaderSource.c_str() };

        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);

            std::cout << type << " shader error: " << infoLog << '\n';
        }

        return shader;
    }
}
