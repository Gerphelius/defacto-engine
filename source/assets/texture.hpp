#pragma once

namespace DF::Assets
{
    class Texture
    {
    public:
        Texture(unsigned int width, unsigned int height, void* data) noexcept;

        void bind(int pos) const;

    private:
        unsigned int m_texture{};
    };
}
