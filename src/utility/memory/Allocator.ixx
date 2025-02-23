module;

#include <memory>

export module utility.memory.Allocator;

namespace util {

export class Allocator {
public:
    template <typename T>
    [[nodiscard]]
    constexpr static auto allocate(size_t n = 1) -> T*;

    template <typename T, typename... Args_T>
    constexpr static auto construct(T* pointer, Args_T&&...) -> void;

    template <typename T>
    constexpr static auto destroy(T* pointer) -> void;

    template <typename T>
    constexpr static auto deallocate(T* pointer, size_t n = 1) -> void;
};

}   // namespace util

template <typename T>
constexpr auto util::Allocator::allocate(const size_t n) -> T*
{
    return std::allocator<T>{}.allocate(n);
}

template <typename T, typename... Args_T>
constexpr auto util::Allocator::construct(T* const pointer, Args_T&&... args) -> void
{
    std::construct_at(pointer, std::forward<Args_T>(args)...);
}

template <typename T>
constexpr auto util::Allocator::destroy(T* const pointer) -> void
{
    std::destroy_at(pointer);
}

template <typename T>
constexpr auto util::Allocator::deallocate(T* const pointer, const size_t n) -> void
{
    std::allocator<T>{}.deallocate(pointer, n);
}
