module;

#include <concepts>
#include <cstdint>

export module utility.meta.concepts.allocator;

import utility.meta.concepts.decayed;
import utility.meta.concepts.nothrow_movable;

// TODO: remove unnamed namespace when Clang allows it
namespace {
struct Dummy {
    int dummy;
};
}   // namespace

namespace util::meta {

export template <typename T>
concept allocator_c = decayed_c<T> && nothrow_movable_c<T> && requires {
    typename T::value_type;
} && requires(T allocator, typename T::value_type* pointer, size_t n) {
    {
        allocator.allocate(n)
    } -> std::same_as<typename T::value_type*>;
    {
        allocator.deallocate(pointer, n)
    };
};

export template <typename T>
concept generic_allocator_c = decayed_c<T> && nothrow_movable_c<T>
                           && requires(T allocator, ::Dummy* pointer, size_t n) {
                                  {
                                      allocator.template allocate<::Dummy>(n)
                                  } -> std::same_as<::Dummy*>;
                                  {
                                      allocator.deallocate(pointer, n)
                                  };
                              };

}   // namespace util::meta
