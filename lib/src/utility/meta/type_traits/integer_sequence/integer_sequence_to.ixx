module;

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <type_traits>
#endif

export module utility.meta.type_traits.integer_sequence.integer_sequence_to;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

export template <integer_sequence_c From_T, template <typename T, T...> typename To_T>
struct integer_sequence_to;

template <
    template <typename T_, T_...> typename From_T,
    typename Integer_T,
    Integer_T... integers_T,
    template <typename T_, T_...> typename To_T>
struct integer_sequence_to<From_T<Integer_T, integers_T...>, To_T> {
    using type = To_T<Integer_T, integers_T...>;
};

export template <integer_sequence_c From_T, template <typename T, T...> typename To_T>
using integer_sequence_to_t = typename integer_sequence_to<From_T, To_T>::type;

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

namespace {
template <typename T, T...>
struct Sequence0 {};

template <typename T, T...>
struct Sequence1 {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::integer_sequence_to_t<Sequence0<int, 0, 1>, Sequence1>,
              Sequence1<int, 0, 1>>);

#endif
