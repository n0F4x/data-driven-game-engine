module;

#include <cstdint>

export module utility.meta.reflection.type_id;

// TODO: use reflection

template <size_t>
struct is_instantiated {
    consteval friend auto maybe_defined(is_instantiated);
};

template <size_t counter_T>
struct instantiator {
    consteval friend auto maybe_defined(is_instantiated<counter_T>){};
};

template <size_t counter_T, typename T>
consteval auto get_next_id() -> size_t
{
    if constexpr (!requires { maybe_defined(is_instantiated<counter_T>{}); }) {
        static_assert(sizeof(instantiator<counter_T>));
        return counter_T;
    }
    else {
        return get_next_id<counter_T + 1, T>();
    }
}

template <typename T>
consteval auto get_id_for_type() -> size_t
{
    constexpr size_t result{ get_next_id<0, T>() };
    return result;
}

namespace util::meta {

export template <typename T>
constexpr size_t id_v{ get_id_for_type<T>() };

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(util::meta::id_v<int> == 0);
static_assert(util::meta::id_v<int> == 0);
static_assert(util::meta::id_v<float> == 1);
static_assert(util::meta::id_v<float> == 1);
static_assert(util::meta::id_v<int> == 0);

using MyInt = int;
static_assert(util::meta::id_v<MyInt> == 0);

#endif
