module;

#include <concepts>

export module utility.meta.concepts.allocator;

enum class Dummy {
};

namespace util::meta {

export template <typename T>
concept allocator_c = requires(T allocator, ::Dummy* dummy) {
    {
        allocator.template allocate<::Dummy>()
    } -> std::same_as<::Dummy*>;
    {
        allocator.deallocate(dummy)
    };
};

}   // namespace util::meta
