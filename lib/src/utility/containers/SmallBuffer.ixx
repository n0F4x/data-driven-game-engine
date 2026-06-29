module;

#include <cstddef>
#include <memory>
#include <memory_resource>

export module ddge.utility.containers.SmallBuffer;

import ddge.utility.meta.concepts.decayed;
import ddge.utility.meta.type_traits.const_like;

namespace ddge::util {

export template <typename T, std::size_t size_T, std::size_t alignment_T>
concept fits_in_small_buffer_c = meta::decayed_c<T> && sizeof(T) <= size_T
                              && alignment_T % alignof(T) == 0;

export template <std::size_t size_T, std::size_t alignment_T>
class SmallBuffer {
public:
    SmallBuffer()                   = default;
    SmallBuffer(const SmallBuffer&) = default;
    SmallBuffer(SmallBuffer&&)      = default;
    ~SmallBuffer()                  = default;

    auto operator=(const SmallBuffer&) -> SmallBuffer& = default;
    auto operator=(SmallBuffer&&) -> SmallBuffer&      = default;

    template <fits_in_small_buffer_c<size_T, alignment_T> T>
    [[nodiscard]]
    constexpr auto launder() -> T*
    {
        return std::launder(reinterpret_cast<T*>(m_buffer));
    }

    template <fits_in_small_buffer_c<size_T, alignment_T> T>
    [[nodiscard]]
    constexpr auto launder() const -> const T*
    {
        return std::launder(reinterpret_cast<const T*>(m_buffer));
    }

private:
    alignas(alignment_T) std::byte m_buffer[size_T];
};

}   // namespace ddge::util
