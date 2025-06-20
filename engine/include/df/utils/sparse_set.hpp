#include <vector>

namespace DF::Container
{
    template <typename T>
    class SparseSet
    {
        static constexpr std::size_t null{ static_cast<std::size_t>(-1) };

    private:
        std::vector<T> m_dense{};
        std::vector<std::size_t> m_sparse{};
        std::vector<std::size_t> m_denseToSparse{};

    public:
        void add(std::size_t index, const T& element)
        {
            if (index >= m_sparse.size())
            {
                m_sparse.resize(index + 1, null);
            }

            m_sparse[index] = m_dense.size();
            m_denseToSparse.push_back(index);
            m_dense.push_back(element);
        }

        void remove(std::size_t index)
        {
            if (!contains(index)) return;

            const std::size_t lastDenseIndex{ m_denseToSparse.back() };
            const std::size_t denseIndexToDelete{ m_sparse[index] };

            std::swap(m_dense.back(), m_dense[denseIndexToDelete]);
            std::swap(m_denseToSparse.back(), m_denseToSparse[denseIndexToDelete]);
            std::swap(m_sparse[lastDenseIndex], m_sparse[index]);

            m_dense.pop_back();
            m_denseToSparse.pop_back();
            m_sparse[index] = null;
        }

        T& get(std::size_t index)
        {
            return m_dense[m_sparse[index]];
        }

        const std::vector<T>& getDense() const
        {
            return m_dense;
        }

        const std::vector<std::size_t> getIndices() const
        {
            return m_denseToSparse;
        }

        const std::size_t size() const
        {
            return std::size(m_dense);
        }

        bool contains(std::size_t index)
        {
            return index < m_sparse.size() && m_sparse[index] != null;
        }
    };
}
