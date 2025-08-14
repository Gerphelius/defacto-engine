#pragma once

#include <array>

#include <glad/glad.h>

namespace DF::Assets
{
    class Mesh
    {
    private:
        GLuint m_vbo{};
        GLuint m_ebo{};
        GLuint m_vao{};

        GLsizei m_indicesCount{};

    public:
        template <std::size_t N, std::size_t M>
        explicit Mesh(const std::array<float, N>& vertices, const std::array<int, M>& indices)
            : m_indicesCount{ static_cast<GLsizei>(std::ssize(indices)) }
        {
            glGenBuffers(1, &m_vbo);
            glGenBuffers(1, &m_ebo);
            glGenBuffers(1, &m_vao);

            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // Unbind current vao, vbo and ebo respectively
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void draw() const
        {
            glBindVertexArray(m_vao);
            glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
        }
    };
}
