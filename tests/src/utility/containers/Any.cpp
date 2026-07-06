#include <array>
#include <bit>
#include <cstddef>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

import ddge.utility.containers.Any;
import ddge.utility.memory.Deleter;

namespace ddge::util {

TEST_CASE("ddge::util::Any")
{
    class Value {
    public:
        Value()             = default;
        Value(const Value&) = default;

        constexpr Value(Value&& other) noexcept
            : m_value{ std::exchange(other.m_value, 0) }
        {
        }

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

    STATIC_REQUIRE(std::has_single_bit(sizeof(Any)));
    STATIC_REQUIRE(std::has_single_bit(alignof(Any)));

    SECTION("in_place construct")
    {
        const Any any{ std::in_place_type<Value>, value.value() };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("forwarding construct")
    {
        const Any any{ value };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("copy construct")
    {
        const Any any{ value };

        const Any copy{ any };   // NOLINT(*-unnecessary-copy-initialization)
        REQUIRE(any_cast<Value>(any) == any_cast<Value>(copy));
    }

    SECTION("move construct")
    {
        Any any{ value };

        const Any moved_to{ std::move(any) };
        REQUIRE(any_cast<Value>(moved_to) == value);
    }

    SECTION("copy assignment")
    {
        const Any any{ value };
        Any       copy{ other_value };

        copy = any;
        REQUIRE(any_cast<Value>(copy) == any_cast<Value>(any));
    }

    SECTION("move assignment")
    {
        Any moved_from{ value };
        Any moved_to{ other_value };

        moved_to = std::move(moved_from);
        REQUIRE(any_cast<Value>(moved_to) == value);

        moved_from = std::move(moved_to);
        REQUIRE(any_cast<Value>(moved_from) == value);
    }

    SECTION("large to small copy assignment")
    {
        const Any large{ std::in_place_type<std::array<Value, 16>> };
        // ReSharper disable once CppEntityAssignedButNoRead
        Any       small{ std::in_place_type<Value> };

        small = large;
    }

    SECTION("small to large copy assignment")
    {
        const Any small{ std::in_place_type<Value> };
        // ReSharper disable once CppEntityAssignedButNoRead
        Any       large{ std::in_place_type<std::array<Value, 16>> };

        large = small;
    }

    SECTION("large to small move assignment")
    {
        Any large{ std::in_place_type<std::array<Value, 16>> };
        // ReSharper disable once CppEntityAssignedButNoRead
        Any small{ std::in_place_type<Value> };

        small = std::move(large);
    }

    SECTION("small to large move assignment")
    {
        Any small{ std::in_place_type<Value> };
        // ReSharper disable once CppEntityAssignedButNoRead
        Any large{ std::in_place_type<std::array<Value, 16>> };

        large = std::move(small);
    }

    SECTION("any_cast &")
    {
        Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), Value&>);
        assert(result == value);
    }

    SECTION("any_cast const&")
    {
        const Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&>);
        REQUIRE(result == value);
    }

    SECTION("any_cast &&")
    {
        Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        const auto result = any_cast<Value>(std::move(any));
        REQUIRE(result == value);
    }

    SECTION("any_cast const&&")
    {
        const Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result
            = any_cast<Value>(std::move(any));   // NOLINT(*-move-const-arg)

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&&>);
        REQUIRE(result == value);
    }

    SECTION("reinterpret_any_cast &")
    {
        constexpr static int original{ 42 };
        Any                  any{ std::in_place_type<int>, original };

        [[maybe_unused]]
        decltype(auto) result = reinterpret_any_cast<float>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), float&>);
        assert(result == reinterpret_cast<const float&>(original));
    }

    SECTION("reinterpret_any_cast const&")
    {
        constexpr static int original{ 42 };
        const Any            any{ std::in_place_type<int>, original };

        [[maybe_unused]]
        decltype(auto) result = reinterpret_any_cast<float>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), const float&>);
        REQUIRE(result == reinterpret_cast<const float&>(original));
    }

    SECTION("reinterpret_any_cast &&")
    {
        constexpr static int original{ 42 };
        Any                  any{ std::in_place_type<int>, original };

        [[maybe_unused]]
        decltype(auto) result = reinterpret_any_cast<float>(std::move(any));

        STATIC_REQUIRE(std::is_same_v<decltype(result), float&&>);
        assert(result == reinterpret_cast<const float&>(original));
    }

    SECTION("reinterpret_any_cast const&&")
    {
        constexpr static int original{ 42 };
        const Any            any{ std::in_place_type<int>, original };

        [[maybe_unused]]
        decltype(auto) result
            = reinterpret_any_cast<float>(std::move(any));   // NOLINT(*-move-const-arg)

        STATIC_REQUIRE(std::is_same_v<decltype(result), const float&&>);
        REQUIRE(result == reinterpret_cast<const float&>(original));
    }

    SECTION("allocator propagation")
    {
        class CountingResource : public std::pmr::memory_resource {
        public:
            [[nodiscard]]
            auto count() const noexcept -> std::size_t
            {
                return m_counter;
            }

        private:
            std::size_t m_counter{};

            auto do_allocate(const std::size_t bytes, const std::size_t alignment)
                -> void* override
            {
                m_counter += bytes;
                return std::pmr::new_delete_resource()->allocate(bytes, alignment);
            }

            auto do_deallocate(
                void* const       p,
                const std::size_t bytes,
                const std::size_t alignment
            ) -> void override
            {
                std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
            }

            [[nodiscard]]
            auto do_is_equal(const std::pmr::memory_resource& other) const noexcept
                -> bool override
            {
                return std::pmr::new_delete_resource()->is_equal(other);
            }
        };

        struct InnerObject {};

        using BigObject = std::array<int, 16>;

        struct Container : BigObject {
            using allocator_type = std::pmr::polymorphic_allocator<>;

            explicit Container(const allocator_type& allocator = {})
                : BigObject{},
                  m_allocator{ allocator },
                  m_data{ m_allocator.new_object<InnerObject>(), Deleter{ m_allocator } }
            {
            }

            [[nodiscard]]
            auto get_allocator() const -> allocator_type
            {
                return m_allocator;
            }

            [[nodiscard]]
            static auto capacity() noexcept -> std::size_t
            {
                return sizeof(InnerObject);
            }

        private:
            allocator_type                        m_allocator;
            std::unique_ptr<InnerObject, Deleter> m_data{
                nullptr,
                Deleter{ m_allocator },
            };
        };

        CountingResource     memory_resource;
        const BasicAny<true> any{
            std::allocator_arg,
            &memory_resource,
            std::in_place_type<Container>,
        };

        REQUIRE(any_cast<Container>(any).get_allocator() == any.get_allocator());
        REQUIRE(
            sizeof(Container) + any_cast<Container>(any).capacity()
            == memory_resource.count()
        );
    }

    SECTION("propagates allocator-awareness")
    {
        std::pmr::monotonic_buffer_resource memory_resource;
        std::pmr::vector<Any>               vector{ &memory_resource };
        vector.emplace_back(value);

        REQUIRE(vector.get_allocator() == vector.front().get_allocator());
    }
}

}   // namespace ddge::util
