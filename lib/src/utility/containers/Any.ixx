module;

#include <array>
#include <cassert>
#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "utility/contracts_macros.hpp"

export module ddge.utility.containers.Any;

import ddge.utility.contracts;
import ddge.utility.memory.Allocator;
import ddge.utility.memory.Deallocator;
import ddge.utility.meta.concepts.allocator;
import ddge.utility.meta.concepts.storable;
import ddge.utility.meta.concepts.nothrow_movable;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.hash;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.type_traits.forward_like;

using namespace std::literals;

namespace ddge::util {

// TODO: move to module namespace with better Clang

template <typename T>
concept storable_c = util::meta::storable_c<T> && std::copyable<T>;

template <std::size_t size_T, std::size_t alignment_T>
struct SmallBuffer {
    alignas(alignment_T) std::array<std::byte, size_T> data;

    SmallBuffer() = default;
};

static_assert(std::is_constructible_v<SmallBuffer<64, 8>>);

template <std::size_t size_T, std::size_t alignment_T>
using storage_t = std::variant<SmallBuffer<size_T, alignment_T>, void*>;

template <std::size_t size_T, std::size_t alignment_T, typename Allocator_T>
struct Operations {
    using Storage = storage_t<size_T, alignment_T>;

    using CopyFunc = auto (&)(Storage& out, Allocator_T& allocator, const Storage& storage)
        -> void;
    using MoveFunc       = auto (&)(Storage& out, Storage&& storage) -> void;
    using DropFunc       = auto (&)(Allocator_T& allocator, Storage&&) -> void;
    using TypesMatchFunc = auto (&)(util::meta::TypeHash) -> bool;
    using TypeNameFunc   = auto (&)() -> std::string_view;

    CopyFunc       copy;
    MoveFunc       move;
    DropFunc       drop;
    TypesMatchFunc types_match;
    TypeNameFunc   type_name;
};

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept small_c = storable_c<T> && sizeof(T) <= size_T && alignment_T % alignof(T) == 0
               && util::meta::nothrow_movable_c<T>;

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept large_c = !small_c<T, size_T, alignment_T>;

template <
    typename T,
    std::size_t                     size_T,
    std::size_t                     alignment_T,
    util::meta::generic_allocator_c Allocator_T>
struct Traits;

template <
    typename T,
    std::size_t                     size_T,
    std::size_t                     alignment_T,
    util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
struct Traits<T, size_T, alignment_T, Allocator_T> {
    using Storage     = storage_t<size_T, alignment_T>;
    using SmallBuffer = SmallBuffer<size_T, alignment_T>;

    template <typename... Args_T>
    constexpr static auto create(Storage& out, Allocator_T& allocator, Args_T&&... args)
        -> void;
    constexpr static auto
        copy(Storage& out, Allocator_T& allocator, const Storage& storage) -> void;
    constexpr static auto move(Storage& out, Storage&& storage) noexcept -> void;
    constexpr static auto drop(Allocator_T& allocator, Storage&& storage) noexcept
        -> void;

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    constexpr static auto any_cast(Storage_T&& storage) noexcept
        -> util::meta::forward_like_t<T, Storage_T>;

    [[nodiscard]]
    constexpr static auto types_match(util::meta::TypeHash type_hash) -> bool;
    [[nodiscard]]
    constexpr static auto type_name() -> std::string_view;

    [[nodiscard]]
    constexpr static auto voidify(Storage& storage) -> void*;
    [[nodiscard]]
    constexpr static auto voidify(const Storage& storage) -> const void*;

    constexpr static Operations<size_T, alignment_T, Allocator_T> s_operations{
        .copy        = copy,
        .move        = move,
        .drop        = drop,
        .types_match = types_match,
        .type_name   = type_name,
    };

private:
    template <typename... Args_T>
    constexpr static auto create_impl(Storage& out, Args_T&&... args) -> void;
};

template <
    typename T,
    std::size_t                     size_T,
    std::size_t                     alignment_T,
    util::meta::generic_allocator_c Allocator_T>
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

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    constexpr static auto any_cast(Storage_T&& storage) noexcept
        -> util::meta::forward_like_t<T, Storage_T>;

    [[nodiscard]]
    constexpr static auto types_match(util::meta::TypeHash type_hash) -> bool;
    [[nodiscard]]
    constexpr static auto type_name() -> std::string_view;

    [[nodiscard]]
    constexpr static auto voidify(Storage& storage) -> void*;
    [[nodiscard]]
    constexpr static auto voidify(const Storage& storage) -> const void*;


    constexpr static Operations<size_T, alignment_T, Allocator_T> s_operations{
        .copy        = copy,
        .move        = move,
        .drop        = drop,
        .types_match = types_match,
        .type_name   = type_name,
    };
};

class AnyBase {};

export template <storable_c T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, AnyBase>
constexpr auto any_cast(Any_T&& any) -> ddge::util::meta::forward_like_t<T, Any_T>;

export template <storable_c T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, AnyBase>
constexpr auto dynamic_any_cast(Any_T&& any)
    -> ddge::util::meta::forward_like_t<T, Any_T>;

export template <
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = Allocator>
class BasicAny : public AnyBase {
public:
    constexpr static std::size_t size      = size_T;
    constexpr static std::size_t alignment = alignment_T;
    using Allocator                        = Allocator_T;

    constexpr BasicAny(const BasicAny&);
    constexpr BasicAny(BasicAny&&) noexcept;
    constexpr ~BasicAny();

    template <storable_c T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args);

    template <typename UAllocator_T, storable_c T, typename... Args_T>
        requires std::same_as<std::decay_t<UAllocator_T>, Allocator>
              && std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(
        UAllocator_T&& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    );

    template <typename T>
        requires(!std::same_as<std::decay_t<T>, BasicAny>) && storable_c<std::decay_t<T>>
             && (!meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
    constexpr explicit BasicAny(T&& value);

    template <typename UAllocator_T, typename T>
        requires std::same_as<std::decay_t<UAllocator_T>, Allocator>
              && storable_c<std::decay_t<T>>
              && (!meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
              && std::constructible_from<std::decay_t<T>, T&&>
    constexpr explicit BasicAny(UAllocator_T&& allocator, T&& value);

    constexpr auto operator=(const BasicAny&) -> BasicAny&;
    constexpr auto operator=(BasicAny&&) noexcept -> BasicAny&;

    template <storable_c T, typename Any_T>
        requires std::derived_from<std::remove_cvref_t<Any_T>, AnyBase>
    constexpr friend auto any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>;

    template <storable_c T, typename Any_T>
        requires std::derived_from<std::remove_cvref_t<Any_T>, AnyBase>
    constexpr friend auto dynamic_any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>;

private:
    using Storage = storage_t<size_T, alignment_T>;

    [[no_unique_address]]
    Allocator                                         m_allocator;
    const Operations<size_T, alignment_T, Allocator>* m_operations;
    Storage m_storage{ std::in_place_type<void*> };

    constexpr auto reset() -> void;
};

export using Any = BasicAny<>;

}   // namespace ddge::util

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
template <typename... Args_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::create(
    Storage& out,
    Allocator_T&,
    Args_T&&... args
) -> void
{
    create_impl(out, std::forward<Args_T>(args)...);
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::copy(
    Storage& out,
    Allocator_T&,
    const Storage& storage
) -> void
{
    return create_impl(out, *static_cast<const T*>(voidify(storage)));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::move(
    Storage&  out,
    Storage&& storage
) noexcept -> void
{
    return create_impl(out, std::move(*static_cast<T*>(voidify(storage))));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::drop(
    Allocator_T&,
    Storage&& storage
) noexcept -> void
{
    std::destroy_at(static_cast<T*>(voidify(storage)));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
template <typename Storage_T>
    requires std::same_as<
        std::remove_cvref_t<Storage_T>,
        std::variant<ddge::util::SmallBuffer<size_T, alignment_T>, void*>>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::any_cast(
    Storage_T&& storage
) noexcept -> meta::forward_like_t<T, Storage_T>
{
    using TPtr = std::
        conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;
    using VoidPtr = std::conditional_t<
        std::is_const_v<std::remove_reference_t<Storage_T>>,
        const void*,
        void*>;

    return std::forward_like<Storage_T>(
        *static_cast<TPtr>(static_cast<VoidPtr>(voidify(storage)))
    );
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::types_match(
    const meta::TypeHash type_hash
) -> bool
{
    return type_hash == meta::hash<T>();
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::type_name()
    -> std::string_view
{
    return meta::name_of<T>();
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::voidify(
    Storage& storage
) -> void*
{
    SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
    assert(buffer_ptr != nullptr);
    return static_cast<void*>(buffer_ptr->data.data());
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::voidify(
    const Storage& storage
) -> const void*
{
    const SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
    assert(buffer_ptr != nullptr);
    return static_cast<const void*>(buffer_ptr->data.data());
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::small_c<T, size_T, alignment_T>
template <typename... Args_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::create_impl(
    Storage& out,
    Args_T&&... args
) -> void
{
    SmallBuffer& small_buffer = out.template emplace<SmallBuffer>();

    std::construct_at(
        reinterpret_cast<T*>(small_buffer.data.data()), std::forward<Args_T>(args)...
    );
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
template <typename... Args_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::create(
    Storage&     out,
    Allocator_T& allocator,
    Args_T&&... args
) -> void
{
    using Deallocator = Deallocator<Allocator_T>;

    std::unique_ptr<T, Deallocator> handle{ allocator.template allocate<T>(),
                                            Deallocator{ allocator } };
    std::construct_at(handle.get(), std::forward<Args_T>(args)...);

    out.template emplace<void*>(handle.release());
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::copy(
    Storage&       out,
    Allocator_T&   allocator,
    const Storage& storage
) -> void
{
    return create(out, allocator, *static_cast<const T*>(voidify(storage)));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::move(
    Storage&  out,
    Storage&& storage
) noexcept -> void
{
    out.template emplace<void*>(voidify(storage));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::drop(
    Allocator_T& allocator,
    Storage&&    storage
) noexcept -> void
{
    allocator.deallocate(static_cast<T*>(voidify(storage)));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
template <typename Storage_T>
    requires std::
        same_as<std::remove_cvref_t<Storage_T>, ddge::util::storage_t<size_T, alignment_T>>
    constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::any_cast(
        Storage_T&& storage
    ) noexcept -> meta::forward_like_t<T, Storage_T>
{
    using TPtr = std::
        conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;

    return std::forward_like<Storage_T>(*static_cast<TPtr>(voidify(storage)));
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::types_match(
    const util::meta::TypeHash type_hash
) -> bool
{
    return type_hash == meta::hash<T>();
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::type_name()
    -> std::string_view
{
    return meta::name_of<T>();
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::voidify(
    Storage& storage
) -> void*
{
    auto* const handle_ptr{ std::get_if<void*>(&storage) };
    assert(handle_ptr != nullptr);
    return *handle_ptr;
}

template <
    typename T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires ddge::util::large_c<T, size_T, alignment_T>
constexpr auto ddge::util::Traits<T, size_T, alignment_T, Allocator_T>::voidify(
    const Storage& storage
) -> const void*
{
    const auto* const handle_ptr{ std::get_if<void*>(&storage) };
    assert(handle_ptr != nullptr);
    return *handle_ptr;
}

template <ddge::util::storable_c T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, ddge::util::AnyBase>
constexpr auto ddge::util::any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>
{
    PRECOND(
        any.BasicAny::m_operations->types_match(util::meta::hash<T>()),
        // TODO: use constexpr std::format
        "`Any` has type "s + any.BasicAny::m_operations->type_name()
            + ", but requested type is " + util::meta::name_of<T>()
    );
    return dynamic_any_cast<T>(std::forward<Any_T>(any));
}

template <ddge::util::storable_c T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, ddge::util::AnyBase>
constexpr auto ddge::util::dynamic_any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>
{
    PRECOND(
        any.BasicAny::m_operations != nullptr,
        "Don't use a 'moved-from' (or destroyed) Any!"
    );
    return Traits<
        T,
        std::remove_cvref_t<Any_T>::BasicAny::size,
        std::remove_cvref_t<Any_T>::BasicAny::alignment,
        Allocator>::any_cast(std::forward_like<Any_T>(any.BasicAny::m_storage));
}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    const BasicAny& other
)
    : m_allocator{ other.m_allocator },
      m_operations{ other.m_operations }
{
    PRECOND(other.m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_operations) {
        m_operations->copy(m_storage, m_allocator, other.m_storage);
    }
}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    BasicAny&& other
) noexcept
    : m_allocator{ std::move(other.m_allocator) },
      m_operations{ std::exchange(other.m_operations, nullptr) }
{
    PRECOND(m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_operations) {
        m_operations->move(m_storage, std::move(other.m_storage));
    }
    other.reset();
}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::
    ~BasicAny<size_T, alignment_T, Allocator_T>()
{
    reset();
}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <ddge::util::storable_c T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    std::in_place_type_t<T>,
    Args_T&&... args
)
    : BasicAny{ Allocator{}, std::in_place_type<T>, std::forward<Args_T>(args)... }
{}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, ddge::util::storable_c T, typename... Args_T>
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
              && std::constructible_from<T, Args_T&&...>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
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

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename T>
    requires(!std::same_as<
                std::decay_t<T>,
                ddge::util::BasicAny<size_T, alignment_T, Allocator_T>>)
         && ddge::util::storable_c<std::decay_t<T>>
         && (!ddge::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(T&& value)
    : BasicAny{ std::in_place_type<std::decay_t<T>>, std::forward<T>(value) }
{}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, typename T>
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
          && ddge::util::storable_c<std::decay_t<T>>
          && (!ddge::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
          && std::constructible_from<std::decay_t<T>, T&&>
constexpr ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::BasicAny(
    UAllocator_T&& allocator,
    T&&            value
)
    : BasicAny{ std::forward<UAllocator_T>(allocator),
                std::in_place_type<std::decay_t<T>>,
                std::forward<T>(value) }
{}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr auto ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::operator=(
    const BasicAny& other
) -> BasicAny&
{
    if (&other == this) {
        return *this;
    }

    return *this = BasicAny{ other };
}

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr auto ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::operator=(
    BasicAny&& other
) noexcept -> BasicAny&
{
    PRECOND(other.m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");

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

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr auto ddge::util::BasicAny<size_T, alignment_T, Allocator_T>::reset() -> void
{
    if (m_operations) {
        m_operations->drop(m_allocator, std::move(m_storage));
        m_operations = nullptr;
    }
}
