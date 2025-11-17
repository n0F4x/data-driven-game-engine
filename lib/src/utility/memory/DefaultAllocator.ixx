module;

#include <memory>

export module ddge.utility.memory.DefaultAllocator;

namespace ddge::util {

export class DefaultAllocator {
public:
    template <typename T>
    [[nodiscard]]
    constexpr static auto allocate(std::size_t n = 1) -> T*;

    template <typename T>
    constexpr static auto deallocate(T* pointer, std::size_t n = 1) -> void;
};

}   // namespace ddge::util

template <typename T>
constexpr auto ddge::util::DefaultAllocator::allocate(const std::size_t n) -> T*
{
    return std::allocator<T>{}.allocate(n);
}

template <typename T>
constexpr auto ddge::util::DefaultAllocator::deallocate(T* const pointer, const std::size_t n)
    -> void
{
    std::allocator<T>{}.deallocate(pointer, n);
}
