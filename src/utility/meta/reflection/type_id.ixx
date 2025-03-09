module;

#include <cstdint>

export module utility.meta.reflection.type_id;

// TODO: use reflection

template <size_t, auto>
struct is_instantiated {
    consteval friend auto maybe_defined(is_instantiated);
};

template <size_t counter_T, auto tag_T>
struct instantiator {
    consteval friend auto maybe_defined(is_instantiated<counter_T, tag_T>){};
};

template <size_t counter_T, typename T, auto tag_T>
consteval auto get_next_id() -> size_t
{
    if constexpr (!requires { maybe_defined(is_instantiated<counter_T, tag_T>{}); }) {
        static_assert(sizeof(instantiator<counter_T, tag_T>));
        return counter_T;
    }
    else {
        return get_next_id<counter_T + 1, T, tag_T>();
    }
}

template <typename T, auto tag_T>
consteval auto get_id_for_type() -> size_t
{
    constexpr size_t result{ get_next_id<0, T, tag_T>() };
    return result;
}

namespace util::meta {

export template <auto tag_T = [] {}>
struct type_id_generator_t {
    template <typename T, typename Underlying_T = size_t>
    constexpr static Underlying_T id_v{ get_id_for_type<T, tag_T>() };
};

struct tag_t {};

export template <typename T, typename Underlying_T = size_t>
constexpr Underlying_T id_v{ type_id_generator_t<tag_t{}>::id_v<T> };

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(util::meta::id_v<int> == 0);
static_assert(util::meta::id_v<int> == 0);
static_assert(util::meta::id_v<float> == 1);
static_assert(util::meta::id_v<float> == 1);
static_assert(util::meta::id_v<int> == 0);

using MyInt = int;
static_assert(util::meta::id_v<MyInt> == 0);


using Generator = util::meta::type_id_generator_t<>;
static_assert(Generator::id_v<int> == 0);
static_assert(Generator::id_v<int> == 0);
static_assert(Generator::id_v<float> == 1);
static_assert(Generator::id_v<float> == 1);
static_assert(Generator::id_v<int> == 0);

using MyInt = int;
static_assert(Generator::id_v<MyInt> == 0);


static_assert(util::meta::type_id_generator_t<>::id_v<int> == 0);
static_assert(util::meta::type_id_generator_t<>::id_v<float> == 0);

#endif
