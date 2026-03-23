module;

#include <type_traits>

export module ddge.util.meta.concepts.lvalue_reference;

namespace ddge::util::meta {

export template <typename T>
concept lvalue_reference_c = std::is_lvalue_reference_v<T>;

}   // namespace ddge::util::meta
