module;

#include <type_traits>

export module utility.meta.concepts.nothrow_convertible_to;

namespace util::meta {

export template <typename From_T, typename To_T>
concept nothrow_convertible_to_c = std::is_nothrow_convertible_v<From_T, To_T>
                                && requires {
                                       {
                                           static_cast<To_T>(std::declval<From_T>())
                                       } noexcept;
                                   };

}   // namespace util::meta
