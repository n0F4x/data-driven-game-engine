module;

#include <array>
#include <cassert>
#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <variant>

#include <fmt/compile.h>

#include "utility/contract_macros.hpp"

export module ddge.utility.containers.Any;

import ddge.utility.contracts;
import ddge.utility.memory.DefaultAllocator;
import ddge.utility.memory.Deallocator;
import ddge.utility.meta.concepts.generic_allocator;
import ddge.utility.meta.concepts.storable;
import ddge.utility.meta.concepts.nothrow_movable;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.hash;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.type_traits.always_true;
import ddge.utility.meta.type_traits.forward_like;

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

    using CopyFunc = auto (*)(Storage& out, Allocator_T& allocator, const Storage& storage)
        -> void;
    using MoveFunc       = auto (&)(Storage& out, Storage&& storage) -> void;
    using DropFunc       = auto (&)(Allocator_T& allocator, Storage&&) -> void;
    using TypesMatchFunc = auto (&)(ddge::util::meta::TypeHash) -> bool;
    using TypeNameFunc   = auto (&)() -> std::string_view;

    CopyFunc       copy;
    MoveFunc       move;
    DropFunc       drop;
    TypesMatchFunc types_match;
    TypeNameFunc   type_name;
};

class AnyBase {};

template <typename T>
using PermissiveConceptPolicy = ddge::util::meta::always_true<T>;

namespace ddge::util {

export struct AnyProperties {
    bool copyable{ true };

    template <typename T>
    [[nodiscard]]
    constexpr auto satisfied() const noexcept -> bool
    {
        return copyable ? std::copy_constructible<T> : true;
    }
};

export template <typename T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, ::AnyBase>
          && (std::remove_cvref_t<Any_T>::template is_storable<T>())
constexpr auto any_cast(Any_T&& any) -> ddge::util::meta::forward_like_t<T, Any_T>;

export template <typename T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, ::AnyBase>
          && (std::remove_cvref_t<Any_T>::template is_storable<T>())
constexpr auto dynamic_any_cast(Any_T&& any)
    -> ddge::util::meta::forward_like_t<T, Any_T>;

export template <
    AnyProperties properties_T                        = {},
    template <typename> typename ConceptPolicy_T      = ::PermissiveConceptPolicy,
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = DefaultAllocator>
class BasicAny : public ::AnyBase {
public:
    constexpr static AnyProperties properties{ properties_T };

    template <typename T>
    constexpr static std::integral_constant<
        bool,
        meta::storable_c<T> && properties_T.satisfied<T>() && ConceptPolicy_T<T>::value>
        is_storable;

    constexpr static std::size_t size      = size_T;
    constexpr static std::size_t alignment = alignment_T;
    using Allocator                        = Allocator_T;

    constexpr BasicAny(const BasicAny&)
        requires(properties_T.copyable);
    constexpr BasicAny(BasicAny&&) noexcept;
    constexpr ~BasicAny();

    template <typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args)
        requires(is_storable<T>());

    template <typename UAllocator_T, typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(
        UAllocator_T&& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    )
        requires std::same_as<std::decay_t<UAllocator_T>, Allocator> && (is_storable<T>())
    ;

    template <typename T>
        requires(!std::same_as<std::decay_t<T>, BasicAny>)
             && (!meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
                constexpr explicit BasicAny(T&& value)
                    requires(is_storable<std::decay_t<T>>());

    template <typename UAllocator_T, typename T>
        requires(!meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
                constexpr explicit BasicAny(UAllocator_T&& allocator, T&& value)
                    requires std::same_as<std::decay_t<UAllocator_T>, Allocator>
                          && (is_storable<std::decay_t<T>>());

    constexpr auto operator=(const BasicAny&) -> BasicAny&
        requires(properties_T.copyable);
    constexpr auto operator=(BasicAny&&) noexcept -> BasicAny&;

    template <typename T, typename Any_T>
        requires std::derived_from<std::remove_cvref_t<Any_T>, ::AnyBase>
              && (std::remove_cvref_t<Any_T>::template is_storable<T>())
    constexpr friend auto any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>;

    template <typename T, typename Any_T>
        requires std::derived_from<std::remove_cvref_t<Any_T>, ::AnyBase>
              && (std::remove_cvref_t<Any_T>::template is_storable<T>())
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

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept small_c = sizeof(T) <= size_T && alignment_T % alignof(T) == 0
               && ddge::util::meta::nothrow_movable_c<T>;

template <typename T, std::size_t size_T, std::size_t alignment_T>
concept large_c = !small_c<T, size_T, alignment_T>;

template <
    typename T,
    ddge::util::AnyProperties             properties_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
struct Traits;

template <typename Traits_T, ddge::util::AnyProperties properties_T>
struct TraitsFunctionSelector {
    [[nodiscard]]
    constexpr static auto select_copy_function()
    {
        if constexpr (properties_T.copyable) {
            return &Traits_T::copy;
        }
        else {
            return nullptr;
        }
    }
};

template <
    typename T,
    ddge::util::AnyProperties             properties_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires small_c<T, size_T, alignment_T>
struct Traits<T, properties_T, size_T, alignment_T, Allocator_T> {
    using Storage     = storage_t<size_T, alignment_T>;
    using SmallBuffer = SmallBuffer<size_T, alignment_T>;

    template <typename... Args_T>
    constexpr static auto create(Storage& out, Allocator_T&, Args_T&&... args) -> void
    {
        create_impl(out, std::forward<Args_T>(args)...);
    }

    constexpr static auto copy(Storage& out, Allocator_T&, const Storage& storage) -> void
        requires(properties_T.copyable)
    {
        return create_impl(out, *static_cast<const T*>(voidify(storage)));
    }

    constexpr static auto move(Storage& out, Storage&& storage) noexcept -> void
    {
        return create_impl(out, std::move(*static_cast<T*>(voidify(storage))));
    }

    constexpr static auto drop(Allocator_T&, Storage&& storage) noexcept -> void
    {
        std::destroy_at(static_cast<T*>(voidify(storage)));
    }

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    constexpr static auto any_cast(Storage_T&& storage) noexcept
        -> ddge::util::meta::forward_like_t<T, Storage_T>
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

    [[nodiscard]]
    constexpr static auto types_match(ddge::util::meta::TypeHash type_hash) -> bool
    {
        return type_hash == ddge::util::meta::hash<T>();
    }

    [[nodiscard]]
    constexpr static auto type_name() -> std::string_view
    {
        return ddge::util::meta::name_of<T>();
    }

    [[nodiscard]]
    constexpr static auto voidify(Storage& storage) -> void*
    {
        SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
        assert(buffer_ptr != nullptr);
        return static_cast<void*>(buffer_ptr->data.data());
    }

    [[nodiscard]]
    constexpr static auto voidify(const Storage& storage) -> const void*
    {
        const SmallBuffer* buffer_ptr{ std::get_if<SmallBuffer>(&storage) };
        assert(buffer_ptr != nullptr);
        return static_cast<const void*>(buffer_ptr->data.data());
    }

    constexpr static Operations<size_T, alignment_T, Allocator_T> s_operations{
        .copy = TraitsFunctionSelector<Traits, properties_T>::select_copy_function(),
        .move = move,
        .drop = drop,
        .types_match = types_match,
        .type_name   = type_name,
    };

private:
    template <typename... Args_T>
    constexpr static auto create_impl(Storage& out, Args_T&&... args) -> void
    {
        SmallBuffer& small_buffer = out.template emplace<SmallBuffer>();

        std::construct_at(
            reinterpret_cast<T*>(small_buffer.data.data()), std::forward<Args_T>(args)...
        );
    }
};

template <
    typename T,
    ddge::util::AnyProperties             properties_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
    requires large_c<T, size_T, alignment_T>
struct Traits<T, properties_T, size_T, alignment_T, Allocator_T> {
    using Storage = storage_t<size_T, alignment_T>;

    template <typename... Args_T>
    constexpr static auto create(Storage& out, Allocator_T& allocator, Args_T&&... args)
        -> void
    {
        using Deallocator = ddge::util::Deallocator<Allocator_T>;

        std::unique_ptr<T, Deallocator> handle{ allocator.template allocate<T>(),
                                                Deallocator{ allocator } };
        std::construct_at(handle.get(), std::forward<Args_T>(args)...);

        out.template emplace<void*>(handle.release());
    }

    constexpr static auto
        copy(Storage& out, Allocator_T& allocator, const Storage& storage) -> void
        requires(properties_T.copyable)
    {
        return create(out, allocator, *static_cast<const T*>(voidify(storage)));
    }

    constexpr static auto move(Storage& out, Storage&& storage) noexcept -> void
    {
        out.template emplace<void*>(voidify(storage));
    }

    constexpr static auto drop(Allocator_T& allocator, Storage&& storage) noexcept -> void
    {
        allocator.deallocate(static_cast<T*>(voidify(storage)));
    }

    template <typename Storage_T>
        requires std::same_as<std::remove_cvref_t<Storage_T>, Storage>
    [[nodiscard]]
    constexpr static auto any_cast(Storage_T&& storage) noexcept
        -> ddge::util::meta::forward_like_t<T, Storage_T>
    {
        using TPtr = std::
            conditional_t<std::is_const_v<std::remove_reference_t<Storage_T>>, const T*, T*>;

        return std::forward_like<Storage_T>(*static_cast<TPtr>(voidify(storage)));
    }

    [[nodiscard]]
    constexpr static auto types_match(ddge::util::meta::TypeHash type_hash) -> bool
    {
        return type_hash == ddge::util::meta::hash<T>();
    }

    [[nodiscard]]
    constexpr static auto type_name() -> std::string_view
    {
        return ddge::util::meta::name_of<T>();
    }

    [[nodiscard]]
    constexpr static auto voidify(Storage& storage) -> void*
    {
        auto* const handle_ptr{ std::get_if<void*>(&storage) };
        assert(handle_ptr != nullptr);
        return *handle_ptr;
    }

    [[nodiscard]]
    constexpr static auto voidify(const Storage& storage) -> const void*
    {
        const auto* const handle_ptr{ std::get_if<void*>(&storage) };
        assert(handle_ptr != nullptr);
        return *handle_ptr;
    }

    constexpr static Operations<size_T, alignment_T, Allocator_T> s_operations{
        .copy = TraitsFunctionSelector<Traits, properties_T>::select_copy_function(),
        .move = move,
        .drop = drop,
        .types_match = types_match,
        .type_name   = type_name,
    };
};

template <typename T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, AnyBase>
          && (std::remove_cvref_t<Any_T>::template is_storable<T>())
constexpr auto ddge::util::any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>
{
    using namespace fmt::literals;

    PRECOND(
        any.BasicAny::m_operations->types_match(util::meta::hash<T>()),
        // TODO: use constexpr std::format
        fmt::format(
            "`Any` has type {}, but requested type is {}"_cf,
            any.BasicAny::m_operations->type_name(),
            util::meta::name_of<T>()
        )
    );
    return dynamic_any_cast<T>(std::forward<Any_T>(any));
}

template <typename T, typename Any_T>
    requires std::derived_from<std::remove_cvref_t<Any_T>, AnyBase>
          && (std::remove_cvref_t<Any_T>::template is_storable<T>())
constexpr auto ddge::util::dynamic_any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>
{
    PRECOND(
        any.BasicAny::m_operations != nullptr,
        "Don't use a 'moved-from' (or destroyed) Any!"
    );
    return Traits<
        T,
        std::remove_cvref_t<Any_T>::BasicAny::properties,
        std::remove_cvref_t<Any_T>::BasicAny::size,
        std::remove_cvref_t<Any_T>::BasicAny::alignment,
        DefaultAllocator>::any_cast(std::forward_like<Any_T>(any.BasicAny::m_storage));
}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::BasicAny(
        const BasicAny& other
    )
    requires(properties_T.copyable)
    : m_allocator{ other.m_allocator },
      m_operations{ other.m_operations }
{
    PRECOND(other.m_operations != nullptr, "Don't use a 'moved-from' (or destroyed) Any!");
    if (m_operations) {
        m_operations->copy(m_storage, m_allocator, other.m_storage);
    }
}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::BasicAny(
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
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::
        ~BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>()
{
    reset();
}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::BasicAny(
        std::in_place_type_t<T>,
        Args_T&&... args
    )
    requires(is_storable<T>())
    : BasicAny{ Allocator{}, std::in_place_type<T>, std::forward<Args_T>(args)... }
{}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::BasicAny(
        UAllocator_T&& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    )
    requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T> && (is_storable<T>())
    : m_allocator{ std::forward<UAllocator_T>(allocator) },
      m_operations{
          &Traits<T, properties_T, size_T, alignment_T, Allocator_T>::s_operations
      }
{
    Traits<T, properties_T, size_T, alignment_T, Allocator_T>::create(
        m_storage, m_allocator, std::forward<Args_T>(args)...
    );
}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename T>
    requires(!std::same_as<
                std::decay_t<T>,
                ddge::util::
                    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>>)
         && (!ddge::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
            constexpr ddge::util::
                BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::
                    BasicAny(T&& value)
                requires(is_storable<std::decay_t<T>>())
    : BasicAny{ std::in_place_type<std::decay_t<T>>, std::forward<T>(value) }
{}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
template <typename UAllocator_T, typename T>
    requires(!ddge::util::meta::specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
            constexpr ddge::util::
                BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::
                    BasicAny(UAllocator_T&& allocator, T&& value)
                requires std::same_as<std::decay_t<UAllocator_T>, Allocator_T>
                      && (is_storable<std::decay_t<T>>())
    : BasicAny{ std::forward<UAllocator_T>(allocator),
                std::in_place_type<std::decay_t<T>>,
                std::forward<T>(value) }
{}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr auto ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::operator=(
        const BasicAny& other
    ) -> BasicAny&
    requires(properties_T.copyable)   //
{
    if (&other == this) {
        return *this;
    }

    return *this = BasicAny{ other };
}

template <
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr auto ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::operator=(
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
    ddge::util::AnyProperties properties_T,
    template <typename> typename ConceptPolicy_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T>
constexpr auto ddge::util::
    BasicAny<properties_T, ConceptPolicy_T, size_T, alignment_T, Allocator_T>::reset()
        -> void
{
    if (m_operations) {
        m_operations->drop(m_allocator, std::move(m_storage));
        m_operations = nullptr;
    }
}
