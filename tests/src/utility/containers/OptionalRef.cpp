#include <expected>
#include <optional>

import utility.containers.OptionalRef;

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional_ref{};
        [[maybe_unused]]
        auto optional{ optional_ref.transform([](int&) -> float { return {}; }) };

        static_assert(std::is_same_v<decltype(optional), std::optional<float>>);

        return true;
    }(),
    "transform to optional test failed"
);

static_assert(
    [] {
        int                          value{ 2 };
        const util::OptionalRef<int> optional_ref{ value };
        constexpr static float       other_value{ 3.2f };
        const auto                   optional{ optional_ref.transform([](int&) -> float {
            return other_value;
        }) };

        return optional.value() == other_value;
    }(),
    "transform to optional with value test failed"
);

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional_ref{};
        constexpr static float           other_value{ 3.2f };
        constexpr auto optional{ optional_ref.transform([](int&) -> float {
            return other_value;
        }) };

        return !optional.has_value();
    }(),
    "transform to optional without value test failed"
);

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional{};
        [[maybe_unused]]
        auto expected{ optional.or_else([] -> std::expected<int, float> { return {}; }) };

        static_assert(std::is_same_v<decltype(expected), std::expected<int, float>>);

        return true;
    }(),
    "or_else to expected test failed"
);

static_assert(
    [] {
        int                          value{ 2 };
        const util::OptionalRef<int> optional{ value };
        const auto expected{ optional.or_else([] -> std::expected<int, float> {
            return {};
        }) };

        return expected.value() == value;
    }(),
    "or_else with value test failed"
);

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional{};
        constexpr static float           error{ 6.7f };
        constexpr auto expected{ optional.or_else([] -> std::expected<int, float> {
            return std::unexpected<float>{ error };
        }) };

        return expected.error() == error;
    }(),
    "or_else without value test failed"
);
