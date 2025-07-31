module;

#include <memory>

export module utility.memory.Allocator;

namespace util {

export class Allocator {
public:
    template <typename T>
    [[nodiscard]]
    constexpr static auto allocate(std::size_t n = 1) -> T*;

    template <typename T>
    constexpr static auto deallocate(T* pointer, std::size_t n = 1) -> void;
};

}   // namespace util

template <typename T>
constexpr auto util::Allocator::allocate(const std::size_t n) -> T*
{
    return std::allocator<T>{}.allocate(n);
}

template <typename T>
constexpr auto util::Allocator::deallocate(T* const pointer, const std::size_t n) -> void
{
    std::allocator<T>{}.deallocate(pointer, n);
}
