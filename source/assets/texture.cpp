#include <iostream>

#include <glad/glad.h>

#include "texture.hpp"

namespace DF::Assets
{
    Texture::Texture(unsigned int width, unsigned int height, TextureFormat format, const void* data) noexcept
    {
        int glTexFormat{ GL_RGB };

        if (format == TextureFormat::RGBA)
        {
            glTexFormat = GL_RGBA;
        }

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, glTexFormat, width, height, 0, glTexFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::bind(int pos) const
    {
        if (pos > GL_MAX_TEXTURE_UNITS)
        {
            std::cerr << "Maximum texture unit pos exceeded.";

            return;
        }

        glActiveTexture(GL_TEXTURE0 + pos);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }
}
