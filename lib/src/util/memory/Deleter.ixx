module;

#include <memory_resource>

export module ddge.util.memory.Deleter;

namespace ddge::util {

export class Deleter {
public:
    constexpr explicit Deleter(const std::pmr::polymorphic_allocator<>& allocator)
        : m_allocator{ allocator }
    {
    }

    template <typename T>
    constexpr auto operator()(T* pointer) -> void
    {
        m_allocator.delete_object(pointer);
    }

private:
    std::pmr::polymorphic_allocator<> m_allocator;
};

}   // namespace ddge::util
