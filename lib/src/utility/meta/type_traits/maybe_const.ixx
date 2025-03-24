module;

#include <type_traits>

export module utility.meta.type_traits.maybe_const;

namespace util::meta {

export template <bool is_const_T, typename T>
using maybe_const_t = std::conditional_t<is_const_T, const T, T>;

export template <bool is_const_T>
struct make_maybe_const {
    template <typename T>
    struct maybe_const {
        using type = util::meta::maybe_const_t<is_const_T, T>;
    };
};

}   // namespace util::meta
