module;

#include <concepts>

export module utility.meta.concepts.nothrow_assignable_from;

namespace util::meta {

export template <typename LHS, typename RHS>
concept nothrow_assignable_from_c = std::is_lvalue_reference_v<LHS>
                                 && std::common_reference_with<
                                        const std::remove_reference_t<LHS>&,
                                        const std::remove_reference_t<RHS>&>
                                 && requires(LHS lhs, RHS&& rhs) {
                                        {
                                            lhs = std::forward<RHS>(rhs)
                                        } noexcept -> std::same_as<LHS>;
                                    };

}   // namespace util::meta
