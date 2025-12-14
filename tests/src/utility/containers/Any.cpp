#include <catch2/catch_test_macros.hpp>

import ddge.utility.containers.Any;
import ddge.utility.containers.AnyCopyable;

TEST_CASE("util::Any")
{
    class Value {
    public:
        Value()             = default;
        Value(const Value&) = default;

        constexpr Value(Value&& other) noexcept
            : m_value{ std::exchange(other.m_value, 0) }
        {}

        constexpr explicit Value(const int value) noexcept : m_value{ value } {}

        auto operator=(const Value&) -> Value& = default;

        constexpr auto operator=(Value&& other) noexcept -> Value&
        {
            m_value = std::exchange(other.m_value, 0);
            return *this;
        }

        auto operator<=>(const Value&) const = default;

        [[nodiscard]]
        constexpr auto value() const noexcept -> int
        {
            return m_value;
        }

    private:
        int m_value{};
    };

    constexpr static Value value{ 2 };
    constexpr static Value other_value{ 3 };

    static_assert(std::copyable<ddge::util::Any>);

    static_assert(
        [] {
            ddge::util::BasicAnyCopyable<0> any{ std::in_place_type<Value>,
                                                 value.value() };
            assert(any_cast<Value>(any) == value);

            return true;
        }(),
        "in_place construct test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{
                ddge::util::BasicAnyCopyable<0>::Allocator{},
                std::in_place_type<Value>,
                value.value()
            };
            assert(any_cast<Value>(any) == value);

            return true;
        }(),
        "in_place construct with allocator test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{ value };
            assert(any_cast<Value>(any) == value);

            return true;
        }(),
        "forwarding construct test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{
                ddge::util::BasicAnyCopyable<0>::Allocator{}, value
            };
            assert(any_cast<Value>(any) == value);

            return true;
        }(),
        "forwarding construct with allocator test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{ value };

            const ddge::util::BasicAnyCopyable<0> copy{ any };
            assert(any_cast<Value>(any) == any_cast<Value>(copy));

            return true;
        }(),
        "copy test failed"
    );

    static_assert(
        [] {
            ddge::util::BasicAnyCopyable<0> any{ value };

            const ddge::util::BasicAnyCopyable<0> moved_to{ std::move(any) };
            assert(any_cast<Value>(moved_to) == value);

            return true;
        }(),
        "move test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{ value };
            ddge::util::BasicAnyCopyable<0>       copy{ other_value };

            copy = any;
            assert(any_cast<Value>(copy) == any_cast<Value>(any));

            return true;
        }(),
        "copy assignment test failed"
    );

    static_assert(
        [] {
            ddge::util::BasicAnyCopyable<0> moved_from{ value };
            ddge::util::BasicAnyCopyable<0> moved_to{ other_value };

            moved_to = std::move(moved_from);
            assert(any_cast<Value>(moved_to) == value);

            moved_from = std::move(moved_to);
            assert(any_cast<Value>(moved_from) == value);

            return true;
        }(),
        "move assignment test failed"
    );

    static_assert(
        [] {
            ddge::util::BasicAnyCopyable<0> any{ std::in_place_type<Value>, value };

            [[maybe_unused]]
            decltype(auto) result = any_cast<Value>(any);

            static_assert(std::is_same_v<decltype(result), Value&>);
            assert(result == value);

            return true;
        }(),
        "get & test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{ std::in_place_type<Value>, value };

            [[maybe_unused]]
            decltype(auto) result = any_cast<Value>(any);

            static_assert(std::is_same_v<decltype(result), const Value&>);
            assert(result == value);

            return true;
        }(),
        "get const& test failed"
    );

    static_assert(
        [] {
            ddge::util::BasicAnyCopyable<0> any{ std::in_place_type<Value>, value };

            [[maybe_unused]]
            const auto result = any_cast<Value>(std::move(any));
            assert(result == value);

            decltype(auto) result_after_move = any_cast<Value>(std::move(any));
            static_assert(std::is_same_v<decltype(result_after_move), Value&&>);
            assert(result_after_move == Value{});

            return true;
        }(),
        "get && test failed"
    );

    static_assert(
        [] {
            const ddge::util::BasicAnyCopyable<0> any{ std::in_place_type<Value>, value };

            [[maybe_unused]]
            decltype(auto) result = any_cast<Value>(std::move(any));

            static_assert(std::is_same_v<decltype(result), const Value&&>);
            assert(result == value);

            return true;
        }(),
        "get const&& test failed"
    );

    SECTION("in_place construct")
    {
        ddge::util::Any any{ std::in_place_type<Value>, value.value() };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("in_place construct with allocator")
    {
        const ddge::util::Any any{ ddge::util::Any::Allocator{},
                                   std::in_place_type<Value>,
                                   value.value() };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("forwarding construct")
    {
        const ddge::util::Any any{ value };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("forwarding construct with allocator")
    {
        const ddge::util::Any any{ ddge::util::Any::Allocator{}, value };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("copy construct")
    {
        const ddge::util::Any any{ value };

        const ddge::util::Any copy{ any };
        REQUIRE(any_cast<Value>(any) == any_cast<Value>(copy));
    }

    SECTION("move construct")
    {
        ddge::util::Any any{ value };

        const ddge::util::Any moved_to{ std::move(any) };
        REQUIRE(any_cast<Value>(moved_to) == value);
    }

    SECTION("copy assignment")
    {
        const ddge::util::Any any{ value };
        ddge::util::Any       copy{ other_value };

        copy = any;
        REQUIRE(any_cast<Value>(copy) == any_cast<Value>(any));
    }

    SECTION("move assignment")
    {
        ddge::util::Any moved_from{ value };
        ddge::util::Any moved_to{ other_value };

        moved_to = std::move(moved_from);
        REQUIRE(any_cast<Value>(moved_to) == value);

        moved_from = std::move(moved_to);
        REQUIRE(any_cast<Value>(moved_from) == value);
    }

    SECTION("get &")
    {
        ddge::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), Value&>);
        assert(result == value);
    }

    SECTION("get const&")
    {
        const ddge::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&>);
        REQUIRE(result == value);
    }

    SECTION("get &&")
    {
        ddge::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        const auto result = any_cast<Value>(std::move(any));
        REQUIRE(result == value);

        decltype(auto) result_after_move = any_cast<Value>(std::move(any));
        STATIC_REQUIRE(std::is_same_v<decltype(result_after_move), Value&&>);
        REQUIRE(result_after_move == Value{});
    }

    SECTION("get const&&")
    {
        const ddge::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(std::move(any));

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&&>);
        REQUIRE(result == value);
    }

    SECTION("large to small")
    {
        const ddge::util::Any large{ std::in_place_type<std::array<Value, 16>> };
        ddge::util::Any       small{ std::in_place_type<Value> };

        small = large;
    }

    SECTION("small to large")
    {
        const ddge::util::Any small{ std::in_place_type<Value> };
        ddge::util::Any       large{ std::in_place_type<std::array<Value, 16>> };

        large = small;
    }
}
