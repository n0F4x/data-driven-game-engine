module;

#include <any>
#include <cassert>
#include <concepts>
#include <memory>
#include <utility>

export module utility.containers.Any;

import utility.meta.concepts.allocator;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.memory.Allocator;
import utility.memory.Deallocator;

template <typename T>
concept storable_c = std::copyable<T>;

namespace util {

// TODO: let Allocator_T be small buffer optimized by default
export template <::util::meta::generic_allocator_c Allocator_T = Allocator>
class BasicAny;

template <typename T>
concept specialization_of_any_c =
    meta::specialization_of_c<std::remove_cvref_t<T>, BasicAny>;

export template <typename T, typename Any_T>
    requires std::constructible_from<T, meta::forward_like_t<std::remove_cvref_t<T>, Any_T>>
constexpr auto any_cast(Any_T&& any) noexcept -> T;

export template <::util::meta::generic_allocator_c Allocator_T = Allocator>
class BasicAny {
public:
    using Allocator = Allocator_T;

    constexpr BasicAny(const BasicAny&);
    constexpr BasicAny(BasicAny&&) noexcept;

    template <storable_c T, typename... Args_T>
    constexpr explicit BasicAny(
        std::in_place_type_t<T>,
        std::tuple<Args_T...> args,
        const Allocator&      allocator = Allocator{}
    );

    constexpr ~BasicAny();

    // TODO: assignment operators

private:
    using Handle         = void*;
    using CopyFunc       = auto (*)(Allocator& allocator, Handle) -> Handle;
    using MoveFunc       = auto (*)(Allocator& allocator, Handle) -> Handle;
    using DestroyFunc    = auto (*)(Handle) -> void;
    using DeallocateFunc = auto (*)(Allocator& allocator, Handle) -> void;

    Allocator      m_allocator;
    // TODO: store only one function for all operations
    CopyFunc       m_copy_func;
    MoveFunc       m_move_func;
    DestroyFunc    m_destroy_func;
    DeallocateFunc m_deallocate_func;
    Handle         m_handle;

    template <typename T, typename... Args_T>
    [[nodiscard]]
    constexpr static auto allocate_and_construct(Allocator& allocator, Args_T&&... args)
        -> Handle;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_copy_func() -> CopyFunc;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_move_func() -> MoveFunc;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_destroy_func() -> DestroyFunc;

    template <typename T>
    [[nodiscard]]
    constexpr static auto make_deallocate_func() -> DeallocateFunc;

    template <typename T, typename Any_T>
        requires std::
            constructible_from<T, meta::forward_like_t<std::remove_cvref_t<T>, Any_T>>
        constexpr friend auto any_cast(Any_T&& any) noexcept -> T;
};

export class Any : public BasicAny<> {
    using Base = BasicAny<>;

public:
    template <storable_c T, typename... Args_T>
    constexpr explicit Any(std::in_place_type_t<T>, Args_T&&... args);
};

}   // namespace util

template <typename T, typename Any_T>
    requires std::
        constructible_from<T, util::meta::forward_like_t<std::remove_cvref_t<T>, Any_T>>
    constexpr auto util::any_cast(Any_T&& any) noexcept -> T
{
    static_assert(
        !std::is_same_v<std::remove_reference_t<T>, T>,
        "`any_cast` to value type is error-prone"
    );
    // TODO: assert that types match

    return std::forward_like<Any_T>(*static_cast<std::remove_reference_t<T>*>(any.m_handle
    ));
}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(const BasicAny& other)
    : m_allocator{ other.m_allocator },
      m_copy_func{ other.m_copy_func },
      m_move_func{ other.m_move_func },
      m_destroy_func{ other.m_destroy_func },
      m_deallocate_func{ other.m_deallocate_func },
      m_handle{ m_copy_func(m_allocator, other.m_handle) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(BasicAny&& other) noexcept
    : m_allocator{ other.m_allocator },
      m_copy_func{ other.m_copy_func },
      m_move_func{ other.m_move_func },
      m_destroy_func{ other.m_destroy_func },
      m_deallocate_func{ other.m_deallocate_func },
      m_handle{ std::exchange(other.m_handle, Handle{}) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
template <storable_c T, typename... Args_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(
    std::in_place_type_t<T>,
    std::tuple<Args_T...> args,
    const Allocator&      allocator
)
    : m_allocator{ allocator },
      m_copy_func{ make_copy_func<T>() },
      m_move_func{ make_move_func<T>() },
      m_destroy_func{ make_destroy_func<T>() },
      m_deallocate_func{ make_deallocate_func<T>() },
      m_handle{ allocate_and_construct<T>(m_allocator, std::get<Args_T>(args)...) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<Allocator_T>::~BasicAny<Allocator_T>()
{
    if (m_handle) {
        m_destroy_func(m_handle);
        m_deallocate_func(m_allocator, m_handle);
        m_handle = Handle{};
    }
}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename T, typename... Args_T>
constexpr auto util::BasicAny<Allocator_T>::allocate_and_construct(
    Allocator& allocator,
    Args_T&&... args
) -> Handle
{
    using Deallocator = ::util::Deallocator<Allocator>;
    std::unique_ptr<T, Deallocator> handle{ allocator.template allocate<T>(),
                                            Deallocator{ allocator } };
    std::construct_at(handle.get(), std::forward<Args_T>(args)...);
    return Handle{ handle.release() };
}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_copy_func() -> CopyFunc
{
    return +[](Allocator& allocator, const Handle other) {
        assert(other != nullptr);

        return allocate_and_construct<T>(allocator, *static_cast<const T*>(other));
    };
}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_move_func() -> MoveFunc
{
    return +[](Allocator& allocator, const Handle other) {
        assert(other != nullptr);

        return allocate_and_construct<T>(
            allocator, std::move(*static_cast<const T*>(other))
        );
    };
}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_destroy_func() -> DestroyFunc
{
    return +[](const Handle handle) { std::destroy_at(static_cast<T*>(handle)); };
}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename T>
constexpr auto util::BasicAny<Allocator_T>::make_deallocate_func() -> DeallocateFunc
{
    return +[](Allocator& allocator, const Handle handle) {
        allocator.deallocate(static_cast<T*>(handle));
    };
}

template <storable_c T, typename... Args_T>
constexpr util::Any::Any(std::in_place_type_t<T>, Args_T&&... args)
    : Base{ std::in_place_type<T>, std::forward_as_tuple(std::forward<Args_T>(args)...) }
{}

module :private;

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
