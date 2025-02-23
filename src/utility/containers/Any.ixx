module;

#include <any>
#include <cassert>
#include <concepts>
#include <memory>
#include <utility>

export module utility.containers.Any;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.memory.Allocator;
import utility.memory.Deallocator;

template <typename T>
concept storable_c = std::copyable<T>;

namespace util {

export template <typename Allocator_T = Allocator>
class BasicAny;

template <typename T>
concept specialization_of_any_c =
    meta::specialization_of_c<std::remove_cvref_t<T>, BasicAny>;

export template <typename T, typename Any_T>
    requires specialization_of_any_c<std::remove_cvref_t<Any_T>>
          && std::constructible_from<T, meta::forward_like_t<std::remove_cvref_t<T>, Any_T>>
constexpr auto any_cast(Any_T&& any) -> T;

export template <typename Allocator_T = Allocator>
class BasicAny {
public:
    using Allocator = Allocator_T;

    constexpr BasicAny(const BasicAny&);
    constexpr BasicAny(BasicAny&&) noexcept;

    template <storable_c T, typename... Args_T>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args);

    template <typename T, typename Any_T>
        requires specialization_of_any_c<std::remove_cvref_t<Any_T>>
              && std::constructible_from<
                     T,
                     meta::forward_like_t<std::remove_cvref_t<T>, Any_T>>
    constexpr friend auto any_cast(Any_T&& any) -> T;

private:
    using DeallocateFunc = auto (*)(void*) -> void;
    // TODO: try using std::indirect
    using Handle         = std::unique_ptr<void, DeallocateFunc>;
    using CopyFunc       = auto (*)(const Handle&) -> Handle;
    using MoveFunc       = auto (*)(Handle&&) -> Handle;

    // TODO: small buffer optimization
    Handle   m_handle;
    CopyFunc m_copy_func;
    MoveFunc m_move_func;

    template <typename T, typename... Args_T>
    [[nodiscard]]
    constexpr static auto create(Args_T&&... args) -> Handle;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_copy_func() -> CopyFunc;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_move_func() -> MoveFunc;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_deallocate_func() -> DeallocateFunc;
};

export using Any = BasicAny<>;

}   // namespace util

template <typename Allocator_T>
template <storable_c T, typename... Args_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(std::in_place_type_t<T>, Args_T&&... args)
    : m_handle{ create<T>(std::forward<Args_T>(args)...) },
      m_copy_func{ make_copy_func<T>() },
      m_move_func{ make_move_func<T>() }
{}

template <typename Allocator_T>
template <typename T, typename... Args_T>
constexpr auto util::BasicAny<Allocator_T>::create(Args_T&&... args) -> Handle
{
    using Deallocator = ::util::Deallocator<Allocator>;

    Allocator                       allocator;
    std::unique_ptr<T, Deallocator> tmp{ allocator.template allocate<T>() };
    T*                              handle{ tmp.release() };
    allocator.construct(handle, std::forward<Args_T>(args)...);
    return Handle{ handle, make_deallocate_func<T>() };
}

template <typename Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_copy_func() -> CopyFunc
{
    return +[](const Handle& other) {
        using Deallocator = ::util::Deallocator<Allocator>;

        Allocator                       allocator;
        std::unique_ptr<T, Deallocator> tmp{ allocator.template allocate<T>() };
        T*                              handle{ tmp.release() };
        allocator.construct(handle, *static_cast<const T*>(other.get()));
        return Handle{ handle, make_deallocate_func<T>() };
    };
}

template <typename Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_move_func() -> MoveFunc
{
    return +[](Handle&& other) { return std::move(other); };
}

template <typename Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_deallocate_func() -> DeallocateFunc
{
    return +[](void* const pointer) {
        Allocator allocator;
        allocator.deallocate(static_cast<T*>(pointer));
    };
}

module :private;

template <typename Allocator_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(const BasicAny& other)
    : m_handle{ other.m_copy_func(other.m_handle) },
      m_copy_func{ other.m_copy_func },
      m_move_func{ other.m_move_func }
{}

template <typename Allocator_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(BasicAny&& other) noexcept
    : m_handle{ std::exchange(other.m_handle, nullptr) },
      m_copy_func{ other.m_copy_func },
      m_move_func{ other.m_move_func }
{}

template <typename T, typename Any_T>
    requires util::specialization_of_any_c<std::remove_cvref_t<Any_T>>
          && std::constructible_from<
                 T,
                 util::meta::forward_like_t<std::remove_cvref_t<T>, Any_T>>
constexpr auto util::any_cast(Any_T&& any) -> T
{
    return std::forward_like<Any_T>(
        *static_cast<std::remove_reference_t<T>*>(any.m_handle.get())
    );
}

#ifdef ENGINE_ENABLE_STATIC_TESTS

class Value {
public:
    Value()             = default;
    Value(const Value&) = default;

    constexpr Value(Value&& other) noexcept : m_value{ std::exchange(other.m_value, 0) }
    {}

    explicit constexpr Value(const int value) noexcept : m_value{ value } {}

    auto operator=(const Value&) -> Value& = default;

    auto operator=(Value&& other) noexcept -> Value&
    {
        std::swap(m_value, other.m_value);
        return *this;
    }

    auto operator<=>(const Value&) const = default;

private:
    int m_value{};
};

constexpr Value value{ 2 };

static_assert(
    [] {
        const util::Any any{ std::in_place_type<Value> };
        util::Any       copy{ any };

        return true;
    }(),
    "copy test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value> };
        util::Any moved_any{ std::move(any) };

        return true;
    }(),
    "move test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = util::any_cast<Value>(any);

        static_assert(std::is_same_v<decltype(result), Value>);
        assert(result == value);

        return true;
    }(),
    "any_cast test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = util::any_cast<Value&>(any);

        static_assert(std::is_same_v<decltype(result), Value&>);
        assert(result == value);

        return true;
    }(),
    "any_cast & test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = util::any_cast<const Value&>(any);

        static_assert(std::is_same_v<decltype(result), const Value&>);
        assert(result == value);

        return true;
    }(),
    "any_cast const& test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        const auto result            = util::any_cast<Value&&>(std::move(any));
        const auto result_after_move = util::any_cast<Value&&>(std::move(any));

        static_assert(std::is_same_v<
                      decltype(util::any_cast<Value&&>(std::move(any))),
                      Value&&>);
        assert(result == value);
        assert(result_after_move == Value{});

        return true;
    }(),
    "any_cast && test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = util::any_cast<const Value&&>(std::move(any));

        static_assert(std::is_same_v<decltype(result), const Value&&>);
        assert(result == value);

        return true;
    }(),
    "any_cast const&& test failed"
);

#endif
