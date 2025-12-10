module;

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.result.Result;

import vulkan_hpp;

import ddge.modules.vulkan.result.make_typed_result_code_variant;
import ddge.modules.vulkan.result.result_category;
import ddge.modules.vulkan.result.ResultCategory;
import ddge.modules.vulkan.result.TypedResultCode;
import ddge.utility.contracts;
import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::vulkan {

[[nodiscard]]
constexpr auto ensure_success(const vk::Result result_code) -> vk::Result
{
    PRECOND(result_category(result_code) == ResultCategory::eSuccess);

    return result_code;
}

[[nodiscard]]
constexpr auto ensure_error(const vk::Result result_code) -> vk::Result
{
    PRECOND(result_category(result_code) != ResultCategory::eSuccess);

    return result_code;
}

export template <util::meta::naked_c Value_T, vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T == vk::Result::eSuccess) || ...)
class Result {
    [[nodiscard]]
    constexpr static auto ensure_expected(const vk::Result result_code) -> vk::Result
    {
        PRECOND(((result_code == expected_result_codes_T) || ...));
        return result_code;
    }

public:
    static_assert(!std::is_same_v<Value_T, vk::Result>);

    explicit Result(Value_T&& value, const vk::Result result_code)
        : m_value{ std::move(value) },
          m_result_code{ ensure_success(ensure_expected(result_code)) }
    {}

    explicit Result(const vk::Result result_code)
        : m_result_code{ ensure_error(ensure_expected(result_code)) }
    {}

    template <typename Self_T>
    [[nodiscard]]
    auto value(this Self_T&& self) -> util::meta::forward_like_t<Value_T, Self_T>
    {
        return std::forward_like<Self_T>(*self.m_value);
    }

    [[nodiscard]]
    auto result_code() const noexcept
        -> std::variant<TypedResultCode<expected_result_codes_T>...>
    {
        return make_typed_result_code_variant<expected_result_codes_T...>(m_result_code);
    }

private:
    std::optional<Value_T> m_value;
    vk::Result             m_result_code;
};

}   // namespace ddge::vulkan
