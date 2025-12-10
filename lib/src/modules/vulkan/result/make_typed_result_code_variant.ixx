module;

#include <variant>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.result.make_typed_result_code_variant;

import vulkan_hpp;

import ddge.modules.vulkan.result.TypedResultCode;
import ddge.utility.contracts;
import ddge.utility.meta.algorithms.for_each;

namespace ddge::vulkan {

template <vk::Result expected_result_code_T, vk::Result... expected_result_codes_T>
constexpr auto try_set_result(
    std::variant<TypedResultCode<expected_result_codes_T>...>& result,
    const vk::Result                                           result_code
) -> void
{
    if (expected_result_code_T == result_code) {
        result = TypedResultCode<expected_result_code_T>{};
    }
}

export template <vk::Result... expected_result_codes_T>
[[nodiscard]]
constexpr auto make_typed_result_code_variant(const vk::Result result_code)
    -> std::variant<TypedResultCode<expected_result_codes_T>...>
{
    PRECOND(((result_code == expected_result_codes_T) || ...));

    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        return {};
    }
    else {
        std::variant<TypedResultCode<expected_result_codes_T>...> result;

        // TODO (Clang): use template for
        (try_set_result<expected_result_codes_T>(result, result_code), ...);

        return result;
    }
}

}   // namespace ddge::vulkan
