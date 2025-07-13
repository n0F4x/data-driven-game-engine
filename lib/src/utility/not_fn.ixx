module;

#include <functional>
#include <type_traits>
#include <utility>

export module utility.not_fn;

import utility.meta.concepts.functional;
import utility.meta.concepts.naked;
import utility.meta.type_traits.functional;

template <typename Derived_T, typename Signature_T>
class NotFnInterface;

template <typename Derived_T, typename Result_T, typename... Args_T>
class NotFnInterface<Derived_T, Result_T(Args_T...)> {
public:
    constexpr auto operator()(this Derived_T self, Args_T... args) -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...)> {
public:
    constexpr auto operator()(this Derived_T self, Args_T... args) -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) noexcept> {
public:
    constexpr auto operator()(this Derived_T self, Args_T... args) noexcept -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...)&> {
public:
    constexpr auto operator()(this Derived_T& self, Args_T... args) -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) & noexcept> {
public:
    constexpr auto operator()(this Derived_T& self, Args_T... args) noexcept -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) &&> {
public:
    constexpr auto operator()(this Derived_T&& self, Args_T... args) -> Result_T
    {
        return !std::invoke(std::move(self.m_invocable), std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) && noexcept> {
public:
    constexpr auto operator()(this Derived_T&& self, Args_T... args) noexcept -> Result_T
    {
        return !std::invoke(std::move(self.m_invocable), std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) const> {
public:
    constexpr auto operator()(this const Derived_T self, Args_T... args) -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) const noexcept> {
public:
    constexpr auto operator()(this const Derived_T self, Args_T... args) noexcept
        -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) const&> {
public:
    constexpr auto operator()(this const Derived_T& self, Args_T... args) -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) const & noexcept> {
public:
    constexpr auto operator()(this const Derived_T& self, Args_T... args) noexcept
        -> Result_T
    {
        return !std::invoke(self.m_invocable, std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) const&&> {
public:
    constexpr auto operator()(this const Derived_T&& self, Args_T... args) -> Result_T
    {
        return !std::invoke(std::move(self.m_invocable), std::forward<Args_T>(args)...);
    }
};

template <typename Derived_T, typename Result_T, typename Class, typename... Args_T>
class NotFnInterface<Derived_T, Result_T (Class::*)(Args_T...) const && noexcept> {
public:
    constexpr auto operator()(this const Derived_T&& self, Args_T... args) noexcept
        -> Result_T
    {
        return !std::invoke(std::move(self.m_invocable), std::forward<Args_T>(args)...);
    }
};

template <typename, typename, typename>
struct NotFnInterfaceFromImpl;

template <
    typename Derived_T,
    typename Result_T,
    template <typename...> typename TypeList_T,
    typename... Args_T>
struct NotFnInterfaceFromImpl<Derived_T, Result_T, TypeList_T<Args_T...>> {
    using type = NotFnInterface<Derived_T, Result_T(Args_T...)>;
};

template <typename Invocable_T>
class NotFn;

template <typename Invocable_T>
using not_fn_interface_from_t = typename NotFnInterfaceFromImpl<
    NotFn<Invocable_T>,
    util::meta::result_of_t<Invocable_T>,
    util::meta::arguments_of_t<Invocable_T>>::type;

template <util::meta::function_c Invocable_T>
class NotFn<Invocable_T> : public not_fn_interface_from_t<Invocable_T> {
    using Base = not_fn_interface_from_t<Invocable_T>;

public:
    constexpr explicit NotFn(Invocable_T& invocable) : m_invocable{ invocable } {}

private:
    friend Base;

    std::reference_wrapper<Invocable_T> m_invocable;
};

template <util::meta::member_function_pointer_c Invocable_T>
class NotFn<Invocable_T> : public not_fn_interface_from_t<Invocable_T> {
    using Base = not_fn_interface_from_t<Invocable_T>;

public:
    constexpr explicit NotFn(Invocable_T invocable) : m_invocable{ invocable } {}

private:
    friend Base;

    Invocable_T m_invocable;
};

template <util::meta::unambiguous_functor_c Invocable_T>
    requires util::meta::naked_c<Invocable_T>
class NotFn<Invocable_T> : public not_fn_interface_from_t<Invocable_T> {
    using Base = not_fn_interface_from_t<Invocable_T>;

public:
    template <typename UInvocable_T>
        requires std::constructible_from<Invocable_T, UInvocable_T&&>
    constexpr explicit NotFn(UInvocable_T&& invocable)
        : m_invocable{ std::forward<UInvocable_T>(invocable) }
    {}

private:
    friend Base;

    Invocable_T m_invocable;
};

namespace util {

export template <typename F>
    requires requires { std::type_identity<NotFn<std::remove_cvref_t<F>>>{}; }
constexpr auto not_fn(F&& func) -> NotFn<std::remove_cvref_t<F>>
{
    return NotFn<std::remove_cvref_t<F>>{ std::forward<F>(func) };
}

}   // namespace util
