module;

#include <cassert>
#include <utility>
#include <variant>

#include <vulkan/vulkan.hpp>

#include "utility/contract_macros.hpp"

export module ddge.modules.vulkan.result.check_result;

import vulkan_hpp;

import ddge.modules.vulkan.result.make_typed_result_code_variant;
import ddge.modules.vulkan.result.Result;
import ddge.modules.vulkan.result.result_category;
import ddge.modules.vulkan.result.ResultCategory;
import ddge.modules.vulkan.result.TypedResultCode;
import ddge.modules.vulkan.result.VulkanError;
import ddge.utility.contracts;
import ddge.utility.meta.concepts.naked;

namespace ddge::vulkan {

[[nodiscard]]
constexpr auto represents_contract_violation(vk::Result result_code) noexcept -> bool;

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
struct CheckResult {
    template <util::meta::naked_c Result_T>
    struct ResultType;

    template <>
    struct ResultType<vk::Result> {
        using type = std::conditional_t<
            sizeof...(expected_result_codes_T) == 0,
            void,
            std::variant<
                TypedResultCode<vk::Result::eSuccess>,
                TypedResultCode<expected_result_codes_T>...>>;
    };

    template <typename Value_T>
    struct ResultType<vk::ResultValue<Value_T>> {
        using type = std::conditional_t<
            sizeof...(expected_result_codes_T) == 0,
            Value_T,
            Result<Value_T, vk::Result::eSuccess, expected_result_codes_T...>>;
    };

    template <typename Value_T>
    struct ResultType<std::expected<Value_T, vk::Result>> {
        using type = std::conditional_t<
            sizeof...(expected_result_codes_T) == 0,
            Value_T,
            std::expected<Value_T, std::variant<TypedResultCode<expected_result_codes_T>...>>>;
    };

    template <typename Result_T>
    using result_type_t = typename ResultType<Result_T>::type;

    template <typename Result_T>
    constexpr static auto operator()(Result_T&& result) -> result_type_t<Result_T>;

private:
    [[nodiscard]]
    constexpr static auto make_result_from_error(vk::Result runtime_error_code)
        -> result_type_t<vk::Result>;
    template <typename Value_T>
    [[nodiscard]]
    constexpr static auto make_result_from_error(vk::ResultValue<Value_T>&& result)
        -> result_type_t<vk::ResultValue<Value_T>>;
    template <typename Value_T>
    [[nodiscard]]
    constexpr static auto make_result_from_error(
        std::expected<Value_T, vk::Result>&& result
    ) -> result_type_t<std::expected<Value_T, vk::Result>>;

    [[nodiscard]]
    constexpr static auto make_result_from_value(vk::Result result)
        -> result_type_t<vk::Result>;
    template <typename Value_T>
    [[nodiscard]]
    constexpr static auto make_result_from_value(vk::ResultValue<Value_T>&& result)
        -> result_type_t<vk::ResultValue<Value_T>>;
    template <typename Value_T>
    [[nodiscard]]
    constexpr static auto make_result_from_value(
        std::expected<Value_T, vk::Result>&& result
    ) -> result_type_t<std::expected<Value_T, vk::Result>>;
};

export template <vk::Result... expected_result_codes_T, typename Result_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
constexpr auto check_result(Result_T&& result)
{
    constexpr static CheckResult<expected_result_codes_T...> check;
    return check(std::move(result));
}

}   // namespace ddge::vulkan

namespace ddge::vulkan {

[[nodiscard]]
constexpr auto represents_precondition_violation(const vk::Result result_code) noexcept
    -> bool
{
    return result_category(result_code) == ResultCategory::ePreconditionViolationError;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_precondition_violation(const vk::ResultValue<T>& result) -> bool
{
    return result_category(result.result) == ResultCategory::ePreconditionViolationError;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_precondition_violation(
    const std::expected<T, vk::Result>& result
) -> bool
{
    return !result.has_value()
        && result_category(result.error()) == ResultCategory::ePreconditionViolationError;
}

[[nodiscard]]
constexpr auto represents_internal_contract_violation(
    const vk::Result result_code
) noexcept -> bool
{
    return result_category(result_code)
        == ResultCategory::eInternalContractViolationError;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_internal_contract_violation(const vk::ResultValue<T>& result)
    -> bool
{
    return result_category(result.result)
        == ResultCategory::eInternalContractViolationError;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_internal_contract_violation(
    const std::expected<T, vk::Result>& result
) -> bool
{
    return !result.has_value()
        && result_category(result.error())
               == ResultCategory::eInternalContractViolationError;
}

[[nodiscard]]
constexpr auto represents_runtime_error(const vk::Result result_code) noexcept -> bool
{
    return result_category(result_code) == ResultCategory::eRuntimeError;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_runtime_error(const vk::ResultValue<T>& result) -> bool
{
    return result_category(result.result) == ResultCategory::eRuntimeError;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_runtime_error(const std::expected<T, vk::Result>& result)
    -> bool
{
    return !result.has_value()
        && result_category(result.error()) == ResultCategory::eRuntimeError;
}

[[nodiscard]]
constexpr auto represents_success(const vk::Result result_code) noexcept -> bool
{
    return result_category(result_code) == ResultCategory::eSuccess;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_success(const vk::ResultValue<T>& result) -> bool
{
    return result_category(result.result) == ResultCategory::eSuccess;
}

template <typename T>
[[nodiscard]]
constexpr auto represents_success(const std::expected<T, vk::Result>& result) -> bool
{
    return result.has_value()
        || result_category(result.error()) == ResultCategory::eSuccess;
}

constexpr auto represents_contract_violation(const vk::Result result_code) noexcept
    -> bool
{
    return represents_precondition_violation(result_code)
        || represents_internal_contract_violation(result_code);
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
template <typename Result_T>
constexpr auto CheckResult<expected_result_codes_T...>::operator()(Result_T&& result)
    -> result_type_t<Result_T>
{
    PRECOND(!represents_precondition_violation(result));
    assert(!represents_internal_contract_violation(result));

    if (represents_runtime_error(result)) {
        return make_result_from_error(std::move(result));
    }

    assert(represents_success(result));

    return make_result_from_value(std::move(result));
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
constexpr auto CheckResult<expected_result_codes_T...>::make_result_from_error(
    const vk::Result runtime_error_code
) -> result_type_t<vk::Result>
{
    PRECOND(result_category(runtime_error_code) == ResultCategory::eRuntimeError);

    if (((runtime_error_code != expected_result_codes_T) && ...)) {
        throw VulkanError{ runtime_error_code };
    }

    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        assert(
            false &&
            "an exception should have been already thrown, "
            "or another contract should have been already violated"
        );
        std::unreachable();
    }
    else {
        return make_typed_result_code_variant<
            vk::Result::eSuccess,
            expected_result_codes_T...>(runtime_error_code);
    }
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
template <typename Value_T>
constexpr auto CheckResult<expected_result_codes_T...>::make_result_from_error(
    vk::ResultValue<Value_T>&& result
) -> result_type_t<vk::ResultValue<Value_T>>
{
    const vk::Result runtime_error_code{ result.result };

    PRECOND(result_category(runtime_error_code) == ResultCategory::eRuntimeError);

    if (((runtime_error_code != expected_result_codes_T) && ...)) {
        throw VulkanError{ runtime_error_code };
    }

    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        assert(
            false &&
            "an exception should have been already thrown, "
            "or another contract should have been already violated"
        );
        std::unreachable();
    }
    else {
        return Result<Value_T, vk::Result::eSuccess, expected_result_codes_T...>{
            runtime_error_code
        };
    }
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
template <typename Value_T>
constexpr auto CheckResult<expected_result_codes_T...>::make_result_from_error(
    std::expected<Value_T, vk::Result>&& result
) -> result_type_t<std::expected<Value_T, vk::Result>>
{
    PRECOND(represents_runtime_error(result));

    if (((result.error() != expected_result_codes_T) && ...)) {
        throw VulkanError{ result.error() };
    }

    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        assert(
            false &&
            "an exception should have been already thrown, "
            "or another contract should have been already violated"
        );
        std::unreachable();
    }
    else {
        return std::move(result).transform_error([](const vk::Result result_code) {
            return make_typed_result_code_variant<expected_result_codes_T...>(result_code);
        });
    }
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
constexpr auto CheckResult<expected_result_codes_T...>::make_result_from_value(
    const vk::Result result
) -> result_type_t<vk::Result>
{
    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        return;
    }
    else {
        return make_typed_result_code_variant<
            vk::Result::eSuccess,
            expected_result_codes_T...>(result);
    }
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
template <typename Value_T>
constexpr auto CheckResult<expected_result_codes_T...>::make_result_from_value(
    vk::ResultValue<Value_T>&& result
) -> result_type_t<vk::ResultValue<Value_T>>
{
    PRECOND(result_category(result.result) == ResultCategory::eSuccess);

    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        return std::move(result.value);
    }
    else {
        return Result<Value_T, vk::Result::eSuccess, expected_result_codes_T...>{
            std::move(result.value),
            result.result   //
        };
    }
}

template <vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T != vk::Result::eSuccess) && ...)
         && ((!represents_contract_violation(expected_result_codes_T)) && ...)
template <typename Value_T>
constexpr auto CheckResult<expected_result_codes_T...>::make_result_from_value(
    std::expected<Value_T, vk::Result>&& result
) -> result_type_t<std::expected<Value_T, vk::Result>>
{
    PRECOND(represents_success(result));

    if constexpr (sizeof...(expected_result_codes_T) == 0) {
        return std::move(*result);
    }
    else {
        return std::move(result).transform_error([](const vk::Result result_code) {
            return make_typed_result_code_variant<expected_result_codes_T...>(result_code);
        });
    }
}

}   // namespace ddge::vulkan
