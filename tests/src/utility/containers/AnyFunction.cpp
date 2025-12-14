#include <concepts>
#include <type_traits>
#include <utility>

import ddge.utility.containers.AnyFunction;
import ddge.utility.containers.AnyCopyableFunction;
import ddge.utility.meta.type_traits.mimic;

namespace {

constexpr auto throwing_function() -> void {}

[[maybe_unused]]
constexpr auto noexcept_function() noexcept -> void
{}

}   // namespace

static_assert(
    [] {
        ddge::util::AnyCopyableFunction<void(), 0z>{ throwing_function };

        return true;
    }(),
    "construct from function"
);

namespace {

struct Dummy {
    auto method() -> void {}
};

}   // namespace

static_assert(
    [] {
        ddge::util::AnyCopyableFunction<void(Dummy), 0z>{ &Dummy::method };

        return true;
    }(),
    "construct from member-function-pointer"
);

static_assert(
    [] {
        ddge::util::AnyCopyableFunction<void(), 0z>{ [] {} };

        return true;
    }(),
    "construct from lambda"
);

static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, int>, int>);

static_assert(
    [] {
        ddge::util::AnyCopyableFunction<void(), 0z>{ [] static {} };

        return true;
    }(),
    "construct from static lambda"
);

static_assert(
    std::constructible_from<
        ddge::util::AnyCopyableFunction<void() noexcept, 0z>,
        decltype(noexcept_function)>
        && !std::constructible_from<
            ddge::util::AnyCopyableFunction<void() noexcept, 0z>,
            decltype(throwing_function)>,
    "construct from throwing"
);

static_assert(
    [] {
        ddge::util::AnyCopyableFunction<void(), 0z> any_function{ throwing_function };

        any_function();
        std::move(any_function)();

        return true;
    }(),
    "non-ref call"
);

static_assert(
    []<typename AnyFunction_T> {
        AnyFunction_T any_function{ throwing_function };

        any_function();
        static_assert(not requires { std::move(any_function)(); });

        return true;
    }.operator()<ddge::util::AnyCopyableFunction<void() &, 0z>>(),
    "& call"
);

static_assert(
    []<typename AnyFunction_T> {
        AnyFunction_T any_function{ throwing_function };

        static_assert(not requires { any_function(); });
        std::move(any_function)();

        return true;
    }.operator()<ddge::util::AnyCopyableFunction<void() &&, 0z>>(),
    "&& call"
);

static_assert(
    std::is_nothrow_invocable_v<ddge::util::AnyCopyableFunction<void() noexcept, 0z>>,
    "noexcept call"
);

static_assert(
    !std::is_nothrow_invocable_v<ddge::util::AnyCopyableFunction<void(), 0z>>,
    "throwing call"
);
