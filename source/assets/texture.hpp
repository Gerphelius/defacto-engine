#pragma once

namespace DF::Assets
{
    enum class TextureFormat
    {
        UNKNOWN,
        RGB,
        RGBA,
    };

    class Texture
    {
    public:
        Texture(unsigned int width, unsigned int height, TextureFormat format, const void* data) noexcept;

        void bind(int pos) const;

    private:
        unsigned int m_texture{};
    };
}
