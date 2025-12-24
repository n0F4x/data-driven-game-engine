#include <optional>

import ddge.utility.Bool;
import ddge.utility.containers.OptionalRef;

static_assert(
    [] {
        constexpr ddge::util::Bool boolean{};
        [[maybe_unused]]
        auto optional{ boolean.transform([] -> float { return {}; }) };

        static_assert(std::is_same_v<decltype(optional), std::optional<float>>);

        return true;
    }(),
    "transform to optional test failed"
);

static_assert(
    [] {
        constexpr ddge::util::Bool boolean{ true };
        constexpr static float     other_value{ 3.2f };
        constexpr auto optional{ boolean.transform([] -> float { return other_value; }) };

        return optional.value() == other_value;
    }(),
    "transform to optional with value test failed"
);

static_assert(
    [] {
        constexpr ddge::util::Bool boolean{};
        constexpr static float     other_value{ 3.2f };
        constexpr auto optional{ boolean.transform([] -> float { return other_value; }) };

        return !optional.has_value();
    }(),
    "transform to optional without value test failed"
);

static_assert(
    [] {
        constexpr ddge::util::Bool boolean{};
        constexpr static float     other_value{ 3.2f };
        [[maybe_unused]]
        auto optional{ boolean.transform([] -> const float& { return other_value; }) };

        static_assert(
            std::is_same_v<decltype(optional), ddge::util::OptionalRef<const float>>
        );

        return true;
    }(),
    "transform to OptionalRef test failed"
);

static_assert(
    [] {
        constexpr ddge::util::Bool boolean{ true };
        constexpr static float     other_value{ 3.2f };
        constexpr auto             optional{ boolean.transform([] -> const float& {
            return other_value;
        }) };

        return *optional == other_value;
    }(),
    "transform to OptionalRef with value test failed"
);

static_assert(
    [] {
        constexpr ddge::util::Bool boolean{};
        constexpr static float     other_value{ 3.2f };
        constexpr auto             optional{ boolean.transform([] -> const float& {
            return other_value;
        }) };

        return !optional.has_value();
    }(),
    "transform to OptionalRef without value test failed"
);
