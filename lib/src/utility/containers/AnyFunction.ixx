module;

#include <concepts>
#include <type_traits>
#include <utility>

export module ddge.utility.containers.AnyFunction;

import ddge.utility.containers.Any;
import ddge.utility.memory.DefaultAllocator;
import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.function_pointer;
import ddge.utility.meta.concepts.generic_allocator;
import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.is_noexcept;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.meta.type_traits.functional.Signature;

template <
    ddge::util::AnyProperties             properties_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T,
    typename Signature_T,
    typename... FArgs_T>
class BasicAnyFunctionBase;

namespace ddge::util {

export template <
    meta::function_c                      Signature_T,
    ddge::util::AnyProperties             properties_T = {},
    std::size_t                           size_T       = 3 * sizeof(void*),
    std::size_t                           alignment_T  = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T  = DefaultAllocator>
using AnyFunction = BasicAnyFunctionBase<
    properties_T,
    size_T,
    alignment_T,
    Allocator_T,
    Signature_T,
    meta::arguments_of_t<Signature_T>>;

export template <
    meta::function_c                      Signature_T,
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = DefaultAllocator>
using AnyCopyableFunction = BasicAnyFunctionBase<
    {},
    size_T,
    alignment_T,
    Allocator_T,
    Signature_T,
    meta::arguments_of_t<Signature_T>>;

}   // namespace ddge::util

template <typename Signature_T, typename... FArgs_T>
struct ConceptPolicyHelper {
    using SignatureTraits = ddge::util::meta::Signature<Signature_T>;
    using Result          = ddge::util::meta::result_of_t<Signature_T>;

    template <typename F>
    struct type {
        using Mimiced = SignatureTraits::template mimic_t<std::decay_t<F>>;

        constexpr static bool value =
            ddge::util::meta::is_noexcept_v<Signature_T>
                ? std::is_nothrow_invocable_r_v<Result, Mimiced, FArgs_T...>
                : std::is_invocable_r_v<Result, Mimiced, FArgs_T...>;
    };
};

template <typename T>
using ensure_reference_t = std::conditional_t<std::is_reference_v<T>, T, T&>;

template <
    ddge::util::AnyProperties             properties_T,
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T,
    typename Signature_T,
    template <typename...> typename ArgList_T,
    typename... FArgs_T>
class BasicAnyFunctionBase<
    properties_T,
    size_T,
    alignment_T,
    Allocator_T,
    Signature_T,
    ArgList_T<FArgs_T...>>
    : public ddge::util::BasicAny<
          properties_T,
          ConceptPolicyHelper<Signature_T, FArgs_T...>::template type,
          size_T,
          alignment_T,
          Allocator_T>   //
{
    using Base = ddge::util::BasicAny<
        properties_T,
        ConceptPolicyHelper<Signature_T, FArgs_T...>::template type,
        size_T,
        alignment_T,
        Allocator_T>;
    using SignatureTraits = ddge::util::meta::Signature<Signature_T>;
    using Result          = ddge::util::meta::result_of_t<Signature_T>;

    constexpr static std::
        integral_constant<bool, ddge::util::meta::is_noexcept_v<Signature_T>>
            is_noexcept;

    using InvokeQualifiedSelf = ::ensure_reference_t<
        typename SignatureTraits::template mimic_t<BasicAnyFunctionBase>>;

public:
    template <typename T, typename... Args_T>
        requires std::constructible_from<std::decay_t<T>, Args_T&&...>
    constexpr explicit BasicAnyFunctionBase(std::in_place_type_t<T>, Args_T&&... args)
        requires(Base::template is_storable<T>())
        : Base{ std::in_place_type<std::decay_t<T>>, std::forward<Args_T>(args)... },
          m_call{ Traits<T>::call }
    {}

    template <typename UAllocator_T, typename T, typename... Args_T>
        requires std::constructible_from<std::decay_t<T>, Args_T&&...>
    constexpr explicit BasicAnyFunctionBase(
        UAllocator_T&& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    )
        requires std::same_as<std::decay_t<UAllocator_T>, typename Base::Allocator>
                  && (Base::template is_storable<T>())
        : Base{ std::forward<UAllocator_T>(allocator),
                std::in_place_type<std::decay_t<T>>,
                std::forward<Args_T>(args)... },
          m_call{ Traits<T>::call }
    {}

    template <typename T>
        requires(!std::same_as<std::decay_t<T>, BasicAnyFunctionBase>)
             && (!ddge::util::meta::
                     specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
                constexpr explicit BasicAnyFunctionBase(T&& value)
                    requires(Base::template is_storable<std::decay_t<T>>())
        : Base{ std::forward<T>(value) },
          m_call{ Traits<T>::call }
    {}

    template <typename UAllocator_T, typename T>
        requires(!ddge::util::meta::
                     specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
                constexpr explicit BasicAnyFunctionBase(UAllocator_T&& allocator, T&& value)
                    requires std::same_as<std::decay_t<UAllocator_T>, typename Base::Allocator>
                              && (Base::template is_storable<std::decay_t<T>>())
        : Base{ std::forward<UAllocator_T>(allocator), std::forward<T>(value) },
          m_call{ Traits<T>::call }
    {}

    template <typename Self_T>
        requires(SignatureTraits::template mimics_qualifiers<Self_T &&>)
    constexpr auto operator()(this Self_T&& self, FArgs_T... args) noexcept(is_noexcept())
        -> Result
    {
        return self.template BasicAnyFunctionBase<
            properties_T,
            size_T,
            alignment_T,
            Allocator_T,
            Signature_T,
            ArgList_T<FArgs_T...>>::
            m_call(static_cast<InvokeQualifiedSelf>(self), std::forward<FArgs_T>(args)...);
    }

private:
    template <typename F>
    struct Traits {
        constexpr static auto call(InvokeQualifiedSelf that, FArgs_T... args) noexcept(
            is_noexcept()
        ) -> Result
        {
            if constexpr (std::is_void_v<Result>) {
                std::invoke(
                    ddge::util::any_cast<std::decay_t<F>>(
                        std::forward<InvokeQualifiedSelf>(that)
                    ),
                    std::forward<FArgs_T>(args)...
                );
            }
            else {
                return std::invoke(
                    ddge::util::any_cast<std::decay_t<F>>(
                        std::forward<InvokeQualifiedSelf>(that)
                    ),
                    std::forward<FArgs_T>(args)...
                );
            }
        }
    };

    std::reference_wrapper<
        auto(InvokeQualifiedSelf, FArgs_T...) noexcept(is_noexcept())->Result>
        m_call;
};
