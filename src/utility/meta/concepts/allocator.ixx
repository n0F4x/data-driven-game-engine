module;

#include <concepts>

export module utility.meta.concepts.allocator;

enum class Dummy {
};

namespace util::meta {

export template <typename Allocator_T>
concept allocator_c = requires {
    typename Allocator_T::value_type;
} && requires(Allocator_T allocator, typename Allocator_T::value_type* pointer, size_t n) {
    {
        allocator.allocate(n)
    } -> std::same_as<typename Allocator_T::value_type*>;
    {
        allocator.deallocate(pointer, n)
    };
};

export template <typename T>
concept generic_allocator_c = requires(T allocator, ::Dummy* pointer, size_t n) {
    {
        allocator.template allocate<::Dummy>(n)
    } -> std::same_as<::Dummy*>;
    {
        allocator.deallocate(pointer, n)
    };
};

}   // namespace util::meta
