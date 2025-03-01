module;

#include <any>
#include <cassert>
#include <concepts>
#include <memory>
#include <utility>
#include <variant>

export module utility.containers.Any;

import utility.meta.concepts.allocator;
import utility.meta.concepts.decayed;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.memory.Allocator;
import utility.memory.Deallocator;

namespace util {

export template <::util::meta::generic_allocator_c Allocator_T = Allocator>
class BasicAny;

}   // namespace util

template <typename T>
concept storable_c = std::copyable<T> && util::meta::nothrow_movable_c<T>;

template <typename T>
concept specialization_of_any_c =
    util::meta::specialization_of_c<std::remove_cvref_t<T>, util::BasicAny>;

// TODO: small buffer optimization
using Storage = std::variant<void*>;

template <typename Allocator_T>
struct Operations {
    using CopyFunc = auto (*)(Allocator_T& allocator, const Storage&) -> Storage;
    using MoveFunc = auto (*)(Storage&&) -> Storage;
    using DropFunc = auto (*)(Allocator_T& allocator, Storage&&) -> void;

    CopyFunc copy;
    MoveFunc move;
    DropFunc drop;
};

template <typename T>
concept small_c = false;

template <typename T>
concept large_c = true;

template <typename T, util::meta::decayed_c Allocator_T>
struct Traits;

template <large_c T, util::meta::decayed_c Allocator_T>
struct Traits<T, Allocator_T> {
    template <typename... Args_T>
    constexpr static auto create(Allocator_T& allocator, Args_T&&... args) -> Storage;
    constexpr static auto copy(Allocator_T& allocator, const Storage& storage) -> Storage;
    constexpr static auto move(Storage&& storage) noexcept -> Storage;
    constexpr static auto drop(Allocator_T& allocator, Storage&& storage) noexcept;
    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    constexpr static auto get(Storage_T&& storage) noexcept
        -> util::meta::forward_like_t<T, Storage_T>;

    constexpr static Operations<Allocator_T> s_operations{ copy, move, drop };
};

namespace util {

export template <::util::meta::generic_allocator_c Allocator_T = Allocator>
class BasicAny {
public:
    using Allocator = Allocator_T;

    constexpr BasicAny(const BasicAny&);
    constexpr BasicAny(BasicAny&&) noexcept;

    template <storable_c T, typename... Args_T>
        requires std::constructible_from<T, Args_T...>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args);

    template <typename UAllocator_T, storable_c T, typename... Args_T>
        requires std::same_as<std::decay_t<UAllocator_T>, Allocator>
              && std::constructible_from<T, Args_T...>
    constexpr explicit BasicAny(
        UAllocator_T&& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    );

    template <typename T>
        requires(!std::same_as<std::decay_t<T>, BasicAny>) && storable_c<std::decay_t<T>>
             && (!::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T>
    constexpr explicit BasicAny(T&& value);

    template <typename UAllocator_T, typename T>
        requires std::same_as<std::decay_t<UAllocator_T>, Allocator>
              && storable_c<std::decay_t<T>>
              && (!::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
              && std::constructible_from<std::decay_t<T>, T>
    constexpr explicit BasicAny(UAllocator_T&& allocator, T&& value);

    constexpr ~BasicAny();

    constexpr auto operator=(const BasicAny&) -> BasicAny&;
    constexpr auto operator=(BasicAny&&) noexcept -> BasicAny&;

    template <::util::meta::decayed_c T, typename Self_T>
    constexpr auto get(this Self_T&&) noexcept -> ::util::meta::forward_like_t<T, Self_T>;

private:
    Allocator                      m_allocator;
    const ::Operations<Allocator>* m_operations;
    ::Storage                      m_storage;
};

export using Any = BasicAny<>;

}   // namespace util

template <large_c T, util::meta::decayed_c Allocator_T>
template <typename... Args_T>
constexpr auto Traits<T, Allocator_T>::create(Allocator_T& allocator, Args_T&&... args)
    -> Storage

{
    using Deallocator = ::util::Deallocator<Allocator_T>;
    std::unique_ptr<T, Deallocator> handle{ allocator.template allocate<T>(),
                                            Deallocator{ allocator } };
    std::construct_at(handle.get(), std::forward<Args_T>(args)...);
    return static_cast<void*>(handle.release());
}

template <large_c T, util::meta::decayed_c Allocator_T>
constexpr auto Traits<T, Allocator_T>::copy(Allocator_T& allocator, const Storage& storage)
    -> Storage

{
    void* other{ *std::get_if<void*>(&storage) };
    return create(allocator, *static_cast<const T*>(other));
}

template <large_c T, util::meta::decayed_c Allocator_T>
constexpr auto Traits<T, Allocator_T>::move(Storage&& storage) noexcept -> Storage
{
    return Storage{ std::exchange(*std::get_if<void*>(&storage), nullptr) };
}

template <large_c T, util::meta::decayed_c Allocator_T>
constexpr auto
    Traits<T, Allocator_T>::drop(Allocator_T& allocator, Storage&& storage) noexcept
{
    if (void* const handle{ *std::get_if<void*>(&storage) }; handle != nullptr) {
        allocator.deallocate(static_cast<T*>(handle));
    }
}

template <large_c T, util::meta::decayed_c Allocator_T>
template <typename Storage_T>
    requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
constexpr auto Traits<T, Allocator_T>::get(Storage_T&& storage) noexcept
    -> util::meta::forward_like_t<T, Storage_T>
{
    auto* result{ static_cast<T*>(*std::get_if<void*>(&storage)) };
    return static_cast<util::meta::forward_like_t<T, Storage_T>>(*result);
}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(const BasicAny& other)
    : m_allocator{ other.m_allocator },
      m_operations{ other.m_operations },
      m_storage{ m_operations->copy(m_allocator, other.m_storage) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<Allocator_T>::BasicAny(BasicAny&& other) noexcept
    : m_allocator{ std::move(other.m_allocator) },
      m_operations{ other.m_operations },
      m_storage{ m_operations->move(std::move(other.m_storage)) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
template <storable_c T, typename... Args_T>
    requires std::constructible_from<T, Args_T...>
constexpr util::BasicAny<Allocator_T>::BasicAny(std::in_place_type_t<T>, Args_T&&... args)
    : m_allocator{},
      m_operations{ &Traits<T, Allocator_T>::s_operations },
      m_storage{
          Traits<T, Allocator_T>::create(m_allocator, std::forward<Args_T>(args)...)
      }
{}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, storable_c T, typename... Args_T>
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
              && std::constructible_from<T, Args_T...>
constexpr util::BasicAny<Allocator_T>::BasicAny(
    UAllocator_T&& allocator,
    std::in_place_type_t<T>,
    Args_T&&... args
)
    : m_allocator{ std::forward<UAllocator_T>(allocator) },
      m_operations{ &Traits<T, Allocator_T>::s_operations },
      m_storage{
          Traits<T, Allocator_T>::create(m_allocator, std::forward<Args_T>(args)...)
      }
{}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename T>
    requires(!std::same_as<std::decay_t<T>, util::BasicAny<Allocator_T>>)
         && storable_c<std::decay_t<T>>
         && (!::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T>
constexpr util::BasicAny<Allocator_T>::BasicAny(T&& value)
    : BasicAny{ std::in_place_type<std::decay_t<T>>, std::forward<T>(value) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, typename T>
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
          && storable_c<std::decay_t<T>>
          && (!::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
          && std::constructible_from<std::decay_t<T>, T>
constexpr util::BasicAny<Allocator_T>::BasicAny(UAllocator_T&& allocator, T&& value)
    : BasicAny{ std::forward<UAllocator_T>(allocator),
                std::in_place_type<std::decay_t<T>>,
                std::forward<T>(value) }
{}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<Allocator_T>::~BasicAny<Allocator_T>()
{
    m_operations->drop(m_allocator, std::move(m_storage));
}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr auto util::BasicAny<Allocator_T>::operator=(const BasicAny& other) -> BasicAny&
{
    if (&other == this) {
        return *this;
    }

    return *this = BasicAny{ other };
}

template <::util::meta::generic_allocator_c Allocator_T>
constexpr auto util::BasicAny<Allocator_T>::operator=(BasicAny&& other) noexcept
    -> BasicAny&
{
    if (&other == this) {
        return *this;
    }

    m_operations->drop(m_allocator, std::move(m_storage));

    m_allocator  = std::move(other.m_allocator);
    m_operations = other.m_operations;
    m_storage    = m_operations->move(std::move(other.m_storage));

    return *this;
}

template <::util::meta::generic_allocator_c Allocator_T>
template <::util::meta::decayed_c T, typename Self_T>
constexpr auto util::BasicAny<Allocator_T>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<T, Self_T>
{
    // TODO: assert that types match
    return Traits<T, Allocator>::get(std::forward_like<Self_T>(self.m_storage));
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(std::copyable<util::Any>);

class Value {
public:
    Value()             = default;
    Value(const Value&) = default;

    constexpr Value(Value&& other) noexcept : m_value{ std::exchange(other.m_value, 0) }
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

constexpr Value value{ 2 };
constexpr Value other_value{ 3 };

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value.value() };
        assert(any.get<Value>() == value);
        return true;
    }(),
    "in_place construct test failed"
);

static_assert(
    [] {
        const util::Any any{ util::Any::Allocator{},
                             std::in_place_type<Value>,
                             value.value() };
        assert(any.get<Value>() == value);
        return true;
    }(),
    "in_place construct with allocator test failed"
);

static_assert(
    [] {
        const util::Any any{ value };
        assert(any.get<Value>() == value);
        return true;
    }(),
    "forwarding construct test failed"
);

static_assert(
    [] {
        const util::Any any{ util::Any::Allocator{}, value };
        assert(any.get<Value>() == value);
        return true;
    }(),
    "forwarding construct with allocator test failed"
);

static_assert(
    [] {
        const util::Any any{ value };
        const util::Any copy{ any };
        assert(any.get<Value>() == copy.get<Value>());
        return true;
    }(),
    "copy test failed"
);

static_assert(
    [] {
        util::Any       any{ value };
        const util::Any moved_to{ std::move(any) };
        assert(moved_to.get<Value>() == value);
        return true;
    }(),
    "move test failed"
);

static_assert(
    [] {
        const util::Any any{ value };
        util::Any       copy{ other_value };
        copy = any;
        assert(copy.get<Value>() == any.get<Value>());
        return true;
    }(),
    "copy assignment test failed"
);

static_assert(
    [] {
        util::Any any{ value };
        util::Any moved_to{ other_value };
        moved_to = std::move(any);
        assert(moved_to.get<Value>() == value);
        return true;
    }(),
    "move assignment test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = any.get<Value>();

        static_assert(std::is_same_v<decltype(result), Value&>);
        assert(result == value);

        return true;
    }(),
    "get & test failed"
);

static_assert(
    [] {
        const util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = any.get<Value>();

        static_assert(std::is_same_v<decltype(result), const Value&>);
        assert(result == value);

        return true;
    }(),
    "get const& test failed"
);

static_assert(
    [] {
        util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        const auto     result            = std::move(any).get<Value>();
        decltype(auto) result_after_move = std::move(any).get<Value>();

        static_assert(std::is_same_v<decltype(result_after_move), Value&&>);
        assert(result == value);
        assert(result_after_move == Value{});

        return true;
    }(),
    "get && test failed"
);

static_assert(
    [] {
        const util::Any any{ std::in_place_type<Value>, value };
        [[maybe_unused]]
        decltype(auto) result = std::move(any).get<Value>();

        static_assert(std::is_same_v<decltype(result), const Value&&>);
        assert(result == value);

        return true;
    }(),
    "get const&& test failed"
);

#endif
