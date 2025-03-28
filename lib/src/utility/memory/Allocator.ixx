module;

#include <memory>

export module utility.memory.Allocator;

#ifdef ENGINE_ENABLE_STATIC_TESTS
import utility.meta.concepts.allocator;
#endif

namespace util {

export class Allocator {
public:
    template <typename T>
    [[nodiscard]]
    constexpr static auto allocate(size_t n = 1) -> T*;

    template <typename T>
    constexpr static auto deallocate(T* pointer, size_t n = 1) -> void;
};

}   // namespace util

template <typename T>
constexpr auto util::Allocator::allocate(const size_t n) -> T*
{
    return std::allocator<T>{}.allocate(n);
}

template <typename T>
constexpr auto util::Allocator::deallocate(T* const pointer, const size_t n) -> void
{
    std::allocator<T>{}.deallocate(pointer, n);
}

module: private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(util::meta::generic_allocator_c<util::Allocator>);

#endif
