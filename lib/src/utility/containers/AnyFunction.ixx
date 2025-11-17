module;

#include <concepts>
#include <type_traits>
#include <utility>

export module ddge.utility.containers.AnyFunction;

import ddge.utility.containers.Any;
import ddge.utility.memory.DefaultAllocator;
import ddge.utility.meta.concepts.decayed;
import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.function_pointer;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.generic_allocator;
import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.concepts.storable;
import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.is_noexcept;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.meta.type_traits.functional.Signature;

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T,
    typename Signature_T,
    typename... FArgs_T>
class BasicAnyFunctionBase;

namespace ddge::util {

export template <
    meta::function_c                      Signature_T,
    std::size_t                           size_T      = 3 * sizeof(void*),
    std::size_t                           alignment_T = sizeof(void*),
    ddge::util::meta::generic_allocator_c Allocator_T = DefaultAllocator>
using BasicAnyFunction = BasicAnyFunctionBase<
    size_T,
    alignment_T,
    Allocator_T,
    Signature_T,
    meta::arguments_of_t<Signature_T>>;

export template <meta::function_c Signature_T>
using AnyFunction = BasicAnyFunction<Signature_T>;

}   // namespace ddge::util

template <typename T>
concept storable_c = ddge::util::meta::naked_c<T>
                  && not ddge::util::meta::function_pointer_c<T>
                  && ddge::util::meta::storable_c<std::decay_t<T>>
                  && requires { requires std::copyable<std::decay_t<T>>; };

template <
    std::size_t                           size_T,
    std::size_t                           alignment_T,
    ddge::util::meta::generic_allocator_c Allocator_T,
    typename Signature_T,
    template <typename...> typename ArgList_T,
    typename... FArgs_T>
class BasicAnyFunctionBase<size_T, alignment_T, Allocator_T, Signature_T, ArgList_T<FArgs_T...>>
    : public ddge::util::BasicAny<size_T, alignment_T, Allocator_T>   //
{
    using Base            = ddge::util::BasicAny<size_T, alignment_T, Allocator_T>;
    using SignatureTraits = ddge::util::meta::Signature<Signature_T>;
    using Result          = ddge::util::meta::result_of_t<Signature_T>;

    constexpr static std::
        integral_constant<bool, ddge::util::meta::is_noexcept_v<Signature_T>>
            is_noexcept;

    template <typename F>
    [[nodiscard]]
    constexpr static auto matches_signature() noexcept -> bool
    {
        using Result  = ddge::util::meta::result_of_t<Signature_T>;
        using Mimiced = SignatureTraits::template mimic_t<std::decay_t<F>>;

        if constexpr (ddge::util::meta::is_noexcept_v<Signature_T>) {
            return std::is_nothrow_invocable_r_v<Result, Mimiced, FArgs_T...>;
        }
        else {
            return std::is_invocable_r_v<Result, Mimiced, FArgs_T...>;
        }
    }

public:
    template <::storable_c T, typename... Args_T>
        requires std::constructible_from<std::decay_t<T>, Args_T&&...>
    constexpr explicit BasicAnyFunctionBase(std::in_place_type_t<T>, Args_T&&... args)
        requires(matches_signature<T>())
        : Base{ std::in_place_type<std::decay_t<T>>, std::forward<Args_T>(args)... },
          m_call{ Traits<T>::call }
    {}

    template <typename UAllocator_T, ::storable_c T, typename... Args_T>
        requires std::same_as<std::decay_t<UAllocator_T>, typename Base::Allocator>
              && std::constructible_from<std::decay_t<T>, Args_T&&...>
                 constexpr explicit BasicAnyFunctionBase(
                     UAllocator_T&& allocator,
                     std::in_place_type_t<T>,
                     Args_T&&... args
                 )
                     requires(matches_signature<T>())
        : Base{ std::forward<UAllocator_T>(allocator),
                std::in_place_type<std::decay_t<T>>,
                std::forward<Args_T>(args)... },
          m_call{ Traits<T>::call }
    {}

    template <typename T>
        requires(!std::convertible_to<std::decay_t<T>, BasicAnyFunctionBase>)
             && (!ddge::util::meta::
                     specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
             && ::storable_c<std::remove_cvref_t<T>>
             && std::constructible_from<std::decay_t<T>, T&&>
                constexpr explicit BasicAnyFunctionBase(T&& value)
                    requires(matches_signature<T>())
        : Base{ std::forward<T>(value) },
          m_call{ Traits<T>::call }
    {}

    template <typename UAllocator_T, typename T>
        requires std::same_as<std::decay_t<UAllocator_T>, typename Base::Allocator>
              && (!ddge::util::meta::
                      specialization_of_c<std::decay_t<T>, std::in_place_type_t>)
              && ::storable_c<std::decay_t<T>>
              && std::constructible_from<std::decay_t<T>, T&&>
                 constexpr explicit BasicAnyFunctionBase(
                     UAllocator_T&& allocator,
                     T&&            value
                 )
                     requires(matches_signature<T>())
        : Base{ std::forward<UAllocator_T>(allocator), std::forward<T>(value) },
          m_call{ Traits<T>::call }
    {}

    constexpr auto operator()(
        this SignatureTraits::template mimic_t<BasicAnyFunctionBase> self,
        FArgs_T... args
    ) noexcept(is_noexcept()) -> Result
    {
        return self.m_call(
            std::forward<decltype(self)>(self), std::forward<FArgs_T>(args)...
        );
    }

private:
    template <typename F>
    struct Traits {
        constexpr static auto call(
            SignatureTraits::template mimic_t<BasicAnyFunctionBase> that,
            FArgs_T... args
        ) noexcept(is_noexcept()) -> Result
        {
            return std::invoke(
                ddge::util::any_cast<std::decay_t<F>>(std::forward<decltype(that)>(that)),
                std::forward<FArgs_T>(args)...
            );
        }
    };

    std::reference_wrapper<
        auto(
            typename SignatureTraits::template mimic_t<BasicAnyFunctionBase>,
            FArgs_T...
        ) noexcept(is_noexcept())
            ->Result>
        m_call;
};
