module;

#include <array>
#include <cassert>
#include <concepts>
#include <memory>
#include <utility>
#include <variant>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <string_view>
#endif

#ifdef ENGINE_ENABLE_RUNTIME_TESTS
  #include <catch2/catch_test_macros.hpp>
#endif

export module utility.containers.Any;

import utility.meta.concepts.allocator;
import utility.meta.concepts.decayed;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.memory.Allocator;
import utility.memory.Deallocator;
import utility.meta.reflection.type_id;

template <typename T>
concept storable_c = util::meta::decayed_c<T> && std::copyable<T>;

template <size_t size_T, size_t alignment_T>
struct small_buffer_t {
    alignas(alignment_T) std::array<std::byte, size_T> data;
};

template <size_t size_T, size_t alignment_T>
using storage_t = std::variant<small_buffer_t<size_T, alignment_T>, void*>;

template <size_t size_T, size_t alignment_T, typename Allocator_T>
struct Operations {
    using Storage = storage_t<size_T, alignment_T>;

    using CopyFunc = auto (*)(Storage& out, Allocator_T& allocator, const Storage& storage)
        -> void;
    using MoveFunc       = auto (*)(Storage& out, Storage&& storage) -> void;
    using DropFunc       = auto (*)(Allocator_T& allocator, Storage&&) -> void;
    using TypesMatchFunc = auto (*)(size_t) -> bool;

    CopyFunc       copy;
    MoveFunc       move;
    DropFunc       drop;
    TypesMatchFunc types_match;
};

template <typename T, size_t size_T, size_t alignment_T>
concept small_c = storable_c<T> && sizeof(T) <= size_T && alignment_T % alignof(T) == 0
               && std::is_nothrow_move_constructible_v<T>;

template <typename T, size_t size_T, size_t alignment_T>
concept large_c = !small_c<T, size_T, alignment_T>;

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
struct Traits;

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
struct Traits<T, size_T, alignment_T, Allocator_T> {
    using Storage     = storage_t<size_T, alignment_T>;
    using SmallBuffer = small_buffer_t<size_T, alignment_T>;

    template <typename... Args_T>
    constexpr static auto create(Storage& out, Allocator_T& allocator, Args_T&&... args)
        -> void;
    constexpr static auto
        copy(Storage& out, Allocator_T& allocator, const Storage& storage) -> void;
    constexpr static auto move(Storage& out, Storage&& storage) noexcept -> void;
    constexpr static auto drop(Allocator_T& allocator, Storage&& storage) noexcept
        -> void;

    [[nodiscard]]
    constexpr static auto types_match(size_t type_id) -> bool;
    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    constexpr static auto get(Storage_T&& storage) noexcept
        -> util::meta::forward_like_t<T, Storage_T>;

    constexpr static Operations<size_T, alignment_T, Allocator_T> s_operations{
        copy,
        move,
        drop,
        types_match,
    };

private:
    template <typename... Args_T>
    constexpr static auto create_impl(Storage& out, Args_T&&... args) -> void;
};

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
struct Traits<T, size_T, alignment_T, Allocator_T> {
    using Storage = storage_t<size_T, alignment_T>;

    template <typename... Args_T>
    constexpr static auto create(Storage& out, Allocator_T& allocator, Args_T&&... args)
        -> void;
    constexpr static auto
        copy(Storage& out, Allocator_T& allocator, const Storage& storage) -> void;
    constexpr static auto move(Storage& out, Storage&& storage) noexcept -> void;
    constexpr static auto drop(Allocator_T& allocator, Storage&& storage) noexcept
        -> void;

    [[nodiscard]]
    constexpr static auto types_match(size_t type_id) -> bool;
    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    constexpr static auto get(Storage_T&& storage) noexcept
        -> util::meta::forward_like_t<T, Storage_T>;

    constexpr static Operations<size_T, alignment_T, Allocator_T> s_operations{
        copy,
        move,
        drop,
        types_match,
    };
};

namespace util {

export template <
    size_t                            size_T      = 3 * sizeof(void*),
    size_t                            alignment_T = sizeof(void*),
    ::util::meta::generic_allocator_c Allocator_T = Allocator>
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
    using Storage = ::storage_t<size_T, alignment_T>;

    Allocator                                           m_allocator;
    const ::Operations<size_T, alignment_T, Allocator>* m_operations;
    Storage m_storage{ std::in_place_type<void*> };

    constexpr auto reset() -> void;
};

export using Any = BasicAny<>;

}   // namespace util

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
template <typename... Args_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::create(
    Storage& out,
    Allocator_T&,
    Args_T&&... args
) -> void
{
    create_impl(out, std::forward<Args_T>(args)...);
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::copy(
    Storage& out,
    Allocator_T&,
    const Storage& storage
) -> void
{
    const SmallBuffer* other_ptr{ std::get_if<SmallBuffer>(&storage) };
    assert(other_ptr != nullptr);
    return create_impl(
        out, *static_cast<const T*>(static_cast<const void*>(other_ptr->data.data()))
    );
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::move(
    Storage&  out,
    Storage&& storage
) noexcept -> void
{
    SmallBuffer* other_ptr{ std::get_if<SmallBuffer>(&storage) };
    assert(other_ptr != nullptr);
    return create_impl(
        out, std::move(*static_cast<T*>(static_cast<void*>(other_ptr->data.data())))
    );
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::drop(
    Allocator_T&,
    Storage&& storage
) noexcept -> void
{
    SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
    assert(buffer_ptr != nullptr);
    std::destroy_at(static_cast<T*>(static_cast<void*>(buffer_ptr->data.data())));
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::types_match(
    const size_t type_id
) -> bool
{
    return type_id == ::util::meta::id_v<T>;
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
template <typename Storage_T>
    requires std::same_as<
        std::remove_cvref_t<Storage_T>,
        std::variant<small_buffer_t<size_T, alignment_T>, void*>>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::get(
    Storage_T&& storage
) noexcept -> util::meta::forward_like_t<T, Storage_T>
{
    using TPtr = std::
        conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;
    using VoidPtr = std::conditional_t<
        std::is_const_v<std::remove_reference_t<Storage_T>>,
        const void*,
        void*>;

    auto* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
    assert(buffer_ptr != nullptr);
    return std::forward_like<Storage_T>(
        *static_cast<TPtr>(static_cast<VoidPtr>(buffer_ptr->data.data()))
    );
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
template <typename... Args_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::create_impl(
    Storage& out,
    Args_T&&... args
) -> void
{
    SmallBuffer& small_buffer = out.template emplace<SmallBuffer>();

    std::construct_at(
        static_cast<T*>(static_cast<void*>(small_buffer.data.data())),
        std::forward<Args_T>(args)...
    );
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
template <typename... Args_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::create(
    Storage&     out,
    Allocator_T& allocator,
    Args_T&&... args
) -> void
{
    using Deallocator = ::util::Deallocator<Allocator_T>;

    std::unique_ptr<T, Deallocator> handle{ allocator.template allocate<T>(),
                                            Deallocator{ allocator } };
    std::construct_at(handle.get(), std::forward<Args_T>(args)...);

    out.template emplace<void*>(handle.release());
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::copy(
    Storage&       out,
    Allocator_T&   allocator,
    const Storage& storage
) -> void
{
    void* const* other_ptr{ std::get_if<void*>(&storage) };
    assert(other_ptr != nullptr);
    assert(*other_ptr != nullptr);
    return create(out, allocator, *static_cast<const T*>(*other_ptr));
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::move(
    Storage&  out,
    Storage&& storage
) noexcept -> void
{
    void** other_ptr{ std::get_if<void*>(&storage) };
    assert(other_ptr != nullptr);
    assert(*other_ptr != nullptr);
    out.template emplace<void*>(*other_ptr);
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::drop(
    Allocator_T& allocator,
    Storage&&    storage
) noexcept -> void
{
    void* const* handle_ptr{ std::get_if<void*>(&storage) };
    assert(handle_ptr != nullptr);
    assert(*handle_ptr != nullptr);
    allocator.deallocate(static_cast<T*>(*handle_ptr));
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::types_match(
    const size_t type_id
) -> bool
{
    return type_id == util::meta::id_v<T>;
}

template <typename T, size_t size_T, size_t alignment_T, util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
template <typename Storage_T>
    requires std::same_as<std::remove_cvref_t<Storage_T>, storage_t<size_T, alignment_T>>
constexpr auto Traits<T, size_T, alignment_T, Allocator_T>::get(
    Storage_T&& storage
) noexcept -> util::meta::forward_like_t<T, Storage_T>
{
    using TPtr = std::
        conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;

    auto* handle_ptr{ std::get_if<void*>(&storage) };
    assert(handle_ptr != nullptr);
    return std::forward_like<Storage_T>(*static_cast<TPtr>(*handle_ptr));
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(const BasicAny& other)
    : m_allocator{ other.m_allocator },
      m_operations{ other.m_operations }
{
    assert(
        other.m_operations != nullptr && "Don't use a 'moved-from' (or destroyed) Any!"
    );
    if (m_operations) {
        m_operations->copy(m_storage, m_allocator, other.m_storage);
    }
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    BasicAny&& other
) noexcept
    : m_allocator{ std::move(other.m_allocator) },
      m_operations{ std::exchange(other.m_operations, nullptr) }
{
    assert(m_operations != nullptr && "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_operations) {
        m_operations->move(m_storage, std::move(other.m_storage));
    }
    other.reset();
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
template <storable_c T, typename... Args_T>
    requires std::constructible_from<T, Args_T...>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    std::in_place_type_t<T>,
    Args_T&&... args
)
    : BasicAny{ Allocator{}, std::in_place_type<T>, std::forward<Args_T>(args)... }
{}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, storable_c T, typename... Args_T>
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
              && std::constructible_from<T, Args_T...>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    UAllocator_T&& allocator,
    std::in_place_type_t<T>,
    Args_T&&... args
)
    : m_allocator{ std::forward<UAllocator_T>(allocator) },
      m_operations{ &Traits<T, size_T, alignment_T, Allocator_T>::s_operations }
{
    Traits<T, size_T, alignment_T, Allocator_T>::create(
        m_storage, m_allocator, std::forward<Args_T>(args)...
    );
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
template <typename T>
    requires(!std::
                 same_as<std::decay_t<T>, util::BasicAny<size_T, alignment_T, Allocator_T>>)
         && storable_c<std::decay_t<T>>
         && (!::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(T&& value)
    : BasicAny{ std::in_place_type<std::decay_t<T>>, std::forward<T>(value) }
{}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, typename T>
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
          && storable_c<std::decay_t<T>>
          && (!::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
          && std::constructible_from<std::decay_t<T>, T>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    UAllocator_T&& allocator,
    T&&            value
)
    : BasicAny{ std::forward<UAllocator_T>(allocator),
                std::in_place_type<std::decay_t<T>>,
                std::forward<T>(value) }
{}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
constexpr util::BasicAny<size_T, alignment_T, Allocator_T>::
    ~BasicAny<size_T, alignment_T, Allocator_T>()
{
    reset();
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
constexpr auto util::BasicAny<size_T, alignment_T, Allocator_T>::operator=(
    const BasicAny& other
) -> BasicAny&
{
    if (&other == this) {
        return *this;
    }

    return *this = BasicAny{ other };
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
constexpr auto util::BasicAny<size_T, alignment_T, Allocator_T>::operator=(BasicAny&& other
) noexcept -> BasicAny&
{
    assert(
        other.m_operations != nullptr && "Don't use a 'moved-from' (or destroyed) Any!"
    );

    if (&other == this) {
        return *this;
    }

    reset();

    m_allocator  = std::move(other.m_allocator);
    m_operations = std::exchange(other.m_operations, nullptr);
    if (m_operations) {
        m_operations->move(m_storage, std::move(other.m_storage));
    }

    other.reset();

    return *this;
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
template <::util::meta::decayed_c T, typename Self_T>
constexpr auto util::BasicAny<size_T, alignment_T, Allocator_T>::get(
    this Self_T&& self
) noexcept -> meta::forward_like_t<T, Self_T>
{
    assert(
        self.BasicAny::m_operations != nullptr
        && "Don't use a 'moved-from' (or destroyed) Any!"
    );
    assert(self.BasicAny::m_operations->types_match(util::meta::id_v<T>));
    return Traits<T, size_T, alignment_T, Allocator>::get(
        std::forward_like<Self_T>(self.m_storage)
    );
}

template <size_t size_T, size_t alignment_T, ::util::meta::generic_allocator_c Allocator_T>
constexpr auto util::BasicAny<size_T, alignment_T, Allocator_T>::reset() -> void
{
    if (m_operations) {
        m_operations->drop(m_allocator, std::move(m_storage));
        m_operations = nullptr;
    }
}

module :private;

#if defined ENGINE_ENABLE_STATIC_TESTS or defined ENGINE_ENABLE_RUNTIME_TESTS

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


  #ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(std::copyable<util::Any>);

static_assert(
    [] {
        util::BasicAny<0> any{ std::in_place_type<Value>, value.value() };
        assert(any.get<Value>() == value);

        return true;
    }(),
    "in_place construct test failed"
);
static_assert(
    [] {
        const util::BasicAny<0> any{ util::BasicAny<0>::Allocator{},
                                     std::in_place_type<Value>,
                                     value.value() };
        assert(any.get<Value>() == value);

        return true;
    }(),
    "in_place construct with allocator test failed"
);
static_assert(
    [] {
        const util::BasicAny<0> any{ value };
        assert(any.get<Value>() == value);

        return true;
    }(),
    "forwarding construct test failed"
);
static_assert(
    [] {
        const util::BasicAny<0> any{ util::BasicAny<0>::Allocator{}, value };
        assert(any.get<Value>() == value);

        return true;
    }(),
    "forwarding construct with allocator test failed"
);
static_assert(
    [] {
        const util::BasicAny<0> any{ value };

        const util::BasicAny<0> copy{ any };
        assert(any.get<Value>() == copy.get<Value>());

        return true;
    }(),
    "copy test failed"
);
static_assert(
    [] {
        util::BasicAny<0> any{ value };

        const util::BasicAny<0> moved_to{ std::move(any) };
        assert(moved_to.get<Value>() == value);

        return true;
    }(),
    "move test failed"
);
static_assert(
    [] {
        const util::BasicAny<0> any{ value };
        util::BasicAny<0>       copy{ other_value };

        copy = any;
        assert(copy.get<Value>() == any.get<Value>());

        return true;
    }(),
    "copy assignment test failed"
);
static_assert(
    [] {
        util::BasicAny<0> moved_from{ value };
        util::BasicAny<0> moved_to{ other_value };

        moved_to = std::move(moved_from);
        assert(moved_to.get<Value>() == value);

        moved_from = std::move(moved_to);
        assert(moved_from.get<Value>() == value);

        return true;
    }(),
    "move assignment test failed"
);
static_assert(
    [] {
        util::BasicAny<0> any{ std::in_place_type<Value>, value };

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
        const util::BasicAny<0> any{ std::in_place_type<Value>, value };

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
        util::BasicAny<0> any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        const auto result = std::move(any).get<Value>();
        assert(result == value);

        decltype(auto) result_after_move = std::move(any).get<Value>();
        static_assert(std::is_same_v<decltype(result_after_move), Value&&>);
        assert(result_after_move == Value{});

        return true;
    }(),
    "get && test failed"
);
static_assert(
    [] {
        const util::BasicAny<0> any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = std::move(any).get<Value>();

        static_assert(std::is_same_v<decltype(result), const Value&&>);
        assert(result == value);

        return true;
    }(),
    "get const&& test failed"
);

  #endif

  #ifdef ENGINE_ENABLE_RUNTIME_TESTS

TEST_CASE("util::Any")
{
    SECTION("in_place construct")
    {
        util::Any any{ std::in_place_type<Value>, value.value() };
        REQUIRE(any.get<Value>() == value);
    }

    SECTION("in_place construct with allocator")
    {
        const util::Any any{ util::Any::Allocator{},
                             std::in_place_type<Value>,
                             value.value() };
        REQUIRE(any.get<Value>() == value);
    }

    SECTION("forwarding construct")
    {
        const util::Any any{ value };
        REQUIRE(any.get<Value>() == value);
    }

    SECTION("forwarding construct with allocator")
    {
        const util::Any any{ util::Any::Allocator{}, value };
        REQUIRE(any.get<Value>() == value);
    }

    SECTION("copy construct")
    {
        const util::Any any{ value };

        const util::Any copy{ any };
        REQUIRE(any.get<Value>() == copy.get<Value>());
    }

    SECTION("move construct")
    {
        util::Any any{ value };

        const util::Any moved_to{ std::move(any) };
        REQUIRE(moved_to.get<Value>() == value);
    }

    SECTION("copy assignment")
    {
        const util::Any any{ value };
        util::Any       copy{ other_value };

        copy = any;
        REQUIRE(copy.get<Value>() == any.get<Value>());
    }

    SECTION("move assignment")
    {
        util::Any moved_from{ value };
        util::Any moved_to{ other_value };

        moved_to = std::move(moved_from);
        REQUIRE(moved_to.get<Value>() == value);

        moved_from = std::move(moved_to);
        REQUIRE(moved_from.get<Value>() == value);
    }

    SECTION("get &")
    {
        util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any.get<Value>();

        STATIC_REQUIRE(std::is_same_v<decltype(result), Value&>);
        assert(result == value);
    }

    SECTION("get const&")
    {
        const util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any.get<Value>();

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&>);
        REQUIRE(result == value);
    }

    SECTION("get &&")
    {
        util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        const auto result = std::move(any).get<Value>();
        REQUIRE(result == value);

        decltype(auto) result_after_move = std::move(any).get<Value>();
        STATIC_REQUIRE(std::is_same_v<decltype(result_after_move), Value&&>);
        REQUIRE(result_after_move == Value{});
    }

    SECTION("get const&&")
    {
        const util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = std::move(any).get<Value>();

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&&>);
        REQUIRE(result == value);
    }

    SECTION("large to small")
    {
        const util::Any large{ std::in_place_type<std::array<Value, 16>> };
        util::Any       small{ std::in_place_type<Value> };

        small = large;
    }

    SECTION("small to large")
    {
        const util::Any small{ std::in_place_type<Value> };
        util::Any       large{ std::in_place_type<std::array<Value, 16>> };

        large = small;
    }
}

  #endif

#endif
