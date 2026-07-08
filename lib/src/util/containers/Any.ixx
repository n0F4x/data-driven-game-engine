module;

#include <concepts>
#include <cstddef>
#include <format>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "util/contract_macros.hpp"

export module ddge.util.containers.Any;

import ddge.util.containers.SmallBuffer;
import ddge.util.contracts;
import ddge.util.memory.LifeCycleEraseMechanism;
import ddge.util.meta.concepts.decayed;
import ddge.util.meta.concepts.specialization_of;
import ddge.util.meta.concepts.storable;
import ddge.util.meta.concepts.strips_to;
import ddge.util.meta.reflection.hash;
import ddge.util.meta.reflection.name_of;
import ddge.util.meta.type_traits.always_true;
import ddge.util.meta.type_traits.const_like;
import ddge.util.meta.type_traits.forward_like;

namespace ddge::util {

namespace internal {

class AnyBase {};

}   // namespace internal

export template <typename T>
concept any_c = std::derived_from<T, internal::AnyBase>;

export template <typename T, typename Any_T>
concept storable_in_any_c = any_c<Any_T> && meta::storable_c<T> && meta::decayed_c<T>
                         && (Any_T::is_move_only() || std::copyable<T>)
                         && Any_T::template adheres_to_policy<T>();

export [[nodiscard]]
consteval auto default_any_size() -> std::size_t
{
    return 4 * sizeof(void*);
}

export [[nodiscard]]
consteval auto default_any_alignment() -> std::size_t
{
    return alignof(std::max_align_t);
}

export template <typename T>
using DefaultAnyPolicy = meta::always_true<T>;

export template <meta::decayed_c T, typename Any_T>
    requires storable_in_any_c<T, std::remove_cvref_t<Any_T>>
[[nodiscard]]
constexpr auto any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>;

export template <meta::decayed_c T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
[[nodiscard]]
constexpr auto reinterpret_any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>;

export template <
    bool        is_move_only_T            = false,
    std::size_t size_T                    = default_any_size(),
    std::size_t alignment_T               = default_any_alignment(),
    template <typename> typename Policy_T = DefaultAnyPolicy>
class BasicAny : public internal::AnyBase {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    [[nodiscard]]
    consteval static auto is_move_only() noexcept -> bool;
    [[nodiscard]]
    consteval static auto size() noexcept -> std::size_t;
    [[nodiscard]]
    consteval static auto alignment() noexcept -> std::size_t;
    template <typename T>
    consteval static auto adheres_to_policy() noexcept -> bool;


    constexpr BasicAny(const BasicAny&, const allocator_type& allocator = {})
        requires(!is_move_only_T);
    constexpr BasicAny(BasicAny&&) noexcept;
    constexpr BasicAny(BasicAny&&, const allocator_type& allocator);
    constexpr ~BasicAny();

    template <typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(std::in_place_type_t<T>, Args_T&&... args)
        requires storable_in_any_c<T, BasicAny>;

    template <typename T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit BasicAny(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    )
        requires storable_in_any_c<T, BasicAny>;

    template <typename T>
    constexpr explicit BasicAny(T&& value)
        requires(!meta::strips_to_c<T, BasicAny>)
             && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && storable_in_any_c<std::decay_t<T>, BasicAny>;

    template <typename T>
    constexpr explicit BasicAny(
        std::allocator_arg_t,
        const allocator_type& allocator,
        T&&                   value
    )
        requires(!meta::strips_to_c<T, BasicAny>)
             && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && storable_in_any_c<std::decay_t<T>, BasicAny>;


    constexpr auto operator=(const BasicAny&) -> BasicAny&
        requires(!is_move_only_T);
    constexpr auto operator=(BasicAny&&) noexcept -> BasicAny&
        requires is_move_only_T;
    constexpr auto operator=(BasicAny&&) noexcept -> BasicAny&
        requires(!is_move_only_T);


    [[nodiscard]]
    constexpr auto get_allocator() const noexcept -> allocator_type;


    // ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
    // NOLINTNEXTLINE(*-noexcept-swap)
    constexpr auto swap(BasicAny& other) -> void;


    template <meta::decayed_c T, typename Any_T>
        requires storable_in_any_c<T, std::remove_cvref_t<Any_T>>
    friend constexpr auto any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>;

    template <meta::decayed_c T, typename Any_T>
        requires any_c<std::remove_cvref_t<Any_T>>
    friend constexpr auto reinterpret_any_cast(Any_T&& any)
        -> meta::forward_like_t<T, Any_T>;

private:
    using Storage = SmallBuffer<std::max(size_T, sizeof(void*)), alignment_T>;


    allocator_type                                               m_allocator;
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T> m_erase_mechanism;
    Storage                                                      m_storage;


    constexpr auto reset() -> void;
};

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
// NOLINTNEXTLINE(*-noexcept-swap)
export template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> typename Policy_T>
constexpr auto swap(
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>& lhs,
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>& rhs
) -> void;

export using Any = BasicAny<>;

}   // namespace ddge::util

namespace ddge::util {

template <meta::decayed_c T, typename Any_T>
    requires storable_in_any_c<T, std::remove_cvref_t<Any_T>>
constexpr auto any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>
{
    PRECOND(
        any.BasicAny::m_erase_mechanism.type_hash() == util::meta::hash<T>(),
        std::format(
            "`Any` has type {}, but requested type is {}",
            any.BasicAny::m_erase_mechanism.type_name(),
            util::meta::name_of<T>()
        )
    );

    auto* const address{
        any.BasicAny::m_erase_mechanism.address_of(any.BasicAny::m_storage)
    };

    PRECOND(address != nullptr);

    return std::forward_like<Any_T>(
        // ReSharper disable once CppDFANullDereference
        *static_cast<meta::const_like_t<T, std::remove_reference_t<Any_T>>*>(address)
    );
}

template <meta::decayed_c T, typename Any_T>
    requires any_c<std::remove_cvref_t<Any_T>>
constexpr auto reinterpret_any_cast(Any_T&& any) -> meta::forward_like_t<T, Any_T>
{
    auto* const address{
        any.BasicAny::m_erase_mechanism.address_of(any.BasicAny::m_storage)
    };

    PRECOND(address != nullptr);

    return std::forward_like<Any_T>(
        // ReSharper disable once CppDFANullDereference
        *reinterpret_cast<meta::const_like_t<T, std::remove_reference_t<Any_T>>*>(address)
    );
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
consteval auto
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::is_move_only() noexcept
    -> bool
{
    return is_move_only_T;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
consteval auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::size() noexcept
    -> std::size_t
{
    return size_T;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
consteval auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::alignment() noexcept
    -> std::size_t
{
    return alignment_T;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
template <typename T>
consteval auto
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::adheres_to_policy() noexcept
    -> bool
{
    return Policy_T<T>::value;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(
    const BasicAny&       other,
    const allocator_type& allocator
)
    requires(!is_move_only_T)
    : m_allocator{ allocator },
      m_erase_mechanism{ other.m_erase_mechanism },
      m_storage{ m_erase_mechanism.copy_construct(m_allocator, other.m_storage) }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(
    BasicAny&& other
) noexcept
    : m_allocator{ other.m_allocator },
      m_erase_mechanism{ other.m_erase_mechanism },
      m_storage{ m_erase_mechanism.move_construct(std::move(other.m_storage)) }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(
    BasicAny&&            other,
    const allocator_type& allocator
)
    : m_allocator{ allocator },
      m_erase_mechanism{ other.m_erase_mechanism },
      m_storage{
          m_erase_mechanism
              .move_construct(m_allocator, allocator, std::move(other.m_storage)),
      }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::
    ~BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>()
{
    m_erase_mechanism.drop(m_allocator, m_storage);
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
template <typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(
    std::in_place_type_t<T> in_place_type,
    Args_T&&... args
)
    requires storable_in_any_c<T, BasicAny>
    : BasicAny{
          std::allocator_arg,
          allocator_type{},
          in_place_type,
          std::forward<Args_T>(args)...,
      }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
template <typename T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(
    std::allocator_arg_t,
    const allocator_type&         allocator,
    const std::in_place_type_t<T> in_place_type,
    Args_T&&... args
)
    requires storable_in_any_c<T, BasicAny>
    : m_allocator{ allocator },
      m_erase_mechanism{ std::type_identity<T>{} },
      m_storage{
          m_erase_mechanism.construct(
              std::allocator_arg,
              m_allocator,
              in_place_type,
              std::forward<Args_T>(args)...
          ),
      }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
template <typename T>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(T&& value)
    requires(!meta::strips_to_c<T, BasicAny>)
         && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
         && storable_in_any_c<std::decay_t<T>, BasicAny>
    : BasicAny{
          std::allocator_arg,
          allocator_type{},
          std::forward<T>(value),
      }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
template <typename T>
constexpr BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::BasicAny(
    std::allocator_arg_t,
    const allocator_type& allocator,
    T&&                   value
)
    requires(!meta::strips_to_c<T, BasicAny>)
         && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
         && storable_in_any_c<std::decay_t<T>, BasicAny>
    : BasicAny{
          std::allocator_arg,
          allocator,
          std::in_place_type<std::decay_t<T>>,
          std::forward<T>(value),
      }
{}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::operator=(
    const BasicAny& other
) -> BasicAny&
    requires(!is_move_only_T)
{
    if (this == &other) {
        return *this;
    }

    other.m_erase_mechanism.copy_assign(
        m_allocator, m_storage, m_erase_mechanism, other.m_storage
    );
    m_erase_mechanism = other.m_erase_mechanism;

    return *this;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::operator=(
    BasicAny&& other
) noexcept -> BasicAny&
    requires is_move_only_T
{
    if (this == &other) {
        return *this;
    }

    swap(other);
    other.reset();

    return *this;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::operator=(
    BasicAny&& other
) noexcept -> BasicAny&
    requires(!is_move_only_T)
{
    if (this == &other) {
        return *this;
    }

    if (m_allocator != other.m_allocator) {
        operator=(other);
        return *this;
    }

    swap(other);
    other.reset();

    return *this;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr auto
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::get_allocator() const noexcept
    -> allocator_type
{
    return m_allocator;
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::swap(
    BasicAny& other
) -> void
{
    m_erase_mechanism.swap(
        m_allocator, m_storage, other.m_allocator, other.m_storage, other.m_erase_mechanism
    );
    std::swap(m_erase_mechanism, other.m_erase_mechanism);
}

template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> class Policy_T>
constexpr auto BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>::reset() -> void
{
    m_erase_mechanism.reset(m_allocator, m_storage);
}

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
template <
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T,
    template <typename> typename Policy_T>
constexpr auto swap(
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>& lhs,
    BasicAny<is_move_only_T, size_T, alignment_T, Policy_T>& rhs
) -> void
{
    lhs.swap(rhs);
}

}   // namespace ddge::util
