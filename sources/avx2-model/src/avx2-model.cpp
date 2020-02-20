#include "avx2-model.h"

#include <cassert>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

using namespace std;

namespace matrixmultiplication::avx2
{
    size_t padSize(const size_t size) noexcept
    {
        assert(size > 0);

        return (size - size_t{1}) / NUM_FLOATS_PER_AVX_REGISTER + size_t{1};
    }

    AVXVector::AVXVector(const size_t elements,
                         const float initialValue) noexcept
        : _elements{elements}, _packs(padSize(elements))
    {
        assert(elements > 0);

        for (auto && pack : this->_packs)
        {
            _mm256_store_ps(pack.data(), _mm256_set1_ps(PADDING_VALUE));
        }

        for (size_t i{0}; i < this->_elements; ++i)
        {
            this->at(i) = initialValue;
        }
    }

    std::vector<AVXPack> & AVXVector::packs() noexcept
    {
        return this->_packs;
    }

    const std::vector<AVXPack> & AVXVector::packs() const noexcept
    {
        return this->_packs;
    }

    std::size_t AVXVector::size() const noexcept
    {
        return this->_elements;
    }

    float & AVXVector::at(const size_t i) noexcept
    {
        auto & pack = this->_packs.at(i / NUM_FLOATS_PER_AVX_REGISTER);
        return pack.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }

    float AVXVector::at(const size_t i) const noexcept
    {
        auto pack = this->_packs.at(i / NUM_FLOATS_PER_AVX_REGISTER);
        return pack.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }

    SOAMatrix::SOAMatrix(const size_t rows, const size_t columns,
                         const float initialValue) noexcept
        : _rows{rows}, _columns{columns}, _rowsPaddedSize{padSize(rows)},
          _packs(padSize(rows) * columns)
    {
        assert(rows > 0);
        assert(columns > 0);

        for (auto && pack : this->_packs)
        {
            _mm256_store_ps(pack.data(), _mm256_set1_ps(PADDING_VALUE));
        }

        for (size_t r{0}; r < this->_rows; ++r)
        {
            for (size_t c{0}; c < this->_columns; ++c)
            {
                this->at(r, c) = initialValue;
            }
        }
    }

    std::vector<AVXPack> & SOAMatrix::packs() noexcept
    {
        return this->_packs;
    }

    const std::vector<AVXPack> & SOAMatrix::packs() const noexcept
    {
        return this->_packs;
    }

    std::size_t SOAMatrix::rows() const noexcept
    {
        return this->_rows;
    }

    std::size_t SOAMatrix::columns() const noexcept
    {
        return this->_columns;
    }

    float & SOAMatrix::at(const size_t r, const size_t c) noexcept
    {
        auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->_rowsPaddedSize;
        auto & pack = this->_packs.at(i);
        return pack.at(r % NUM_FLOATS_PER_AVX_REGISTER);
    }

    float SOAMatrix::at(const size_t r, const size_t c) const noexcept
    {
        auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->_rowsPaddedSize;
        auto pack = this->_packs.at(i);
        return pack.at(r % NUM_FLOATS_PER_AVX_REGISTER);
    }
}