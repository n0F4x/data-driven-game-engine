module;

#include <cassert>
#include <memory>
#include <optional>

export module utility.OptionalRef;

namespace util {

template <typename F, typename T>
concept and_then_func_c =
    std::constructible_from<std::invoke_result_t<F, T>, std::nullopt_t>;

template <typename F>
concept or_else_func_c = std::constructible_from<std::invoke_result_t<F>, std::nullopt_t>;

export template <typename T>
class OptionalRef {
public:
    OptionalRef() = default;
    constexpr explicit(false) OptionalRef(std::nullopt_t);
    constexpr explicit(false) OptionalRef(OptionalRef<std::remove_const_t<T>> other)
        requires(std::is_const_v<T>);
    constexpr explicit OptionalRef(T& ref);

    [[nodiscard]]
    constexpr auto operator->() const -> T*;
    [[nodiscard]]
    constexpr auto operator*() const -> T&;

    [[nodiscard]]
    constexpr auto has_value() const -> bool;

    [[nodiscard]]
    constexpr auto value_or(T& other) const -> T&;

    template <typename Self, and_then_func_c<T&> F>
    constexpr auto and_then(this Self&&, F&& func);
    template <typename Self, typename F>
    constexpr auto transform(this Self&&, F&& func);
    template <typename Self, or_else_func_c F>
    constexpr auto or_else(this Self&&, F&& func);

private:
    T* m_handle{};
};

}   // namespace util

template <typename T>
constexpr util::OptionalRef<T>::OptionalRef(std::nullopt_t) : OptionalRef{}
{}

template <typename T>
constexpr util::OptionalRef<T>::OptionalRef(OptionalRef<std::remove_const_t<T>> other)
    requires(std::is_const_v<T>)
    : m_handle{ other.transform([](T& value) { return std::addressof(value); }
      ).value_or(nullptr) }
{}

template <typename T>
constexpr util::OptionalRef<T>::OptionalRef(T& ref) : m_handle{ std::addressof(ref) }
{}

template <typename T>
constexpr auto util::OptionalRef<T>::operator->() const -> T*
{
    assert(m_handle != nullptr);
    return m_handle;
}

template <typename T>
constexpr auto util::OptionalRef<T>::operator*() const -> T&
{
    assert(m_handle != nullptr);
    return *m_handle;
}

template <typename T>
constexpr auto util::OptionalRef<T>::has_value() const -> bool
{
    return m_handle != nullptr;
}

template <typename T>
constexpr auto util::OptionalRef<T>::value_or(T& other) const -> T&
{
    if (has_value()) {
        return *m_handle;
    }
    return other;
}

template <typename T>
template <typename Self, util::and_then_func_c<T&> F>
constexpr auto util::OptionalRef<T>::and_then(this Self&& self, F&& func)
{
    if (self.has_value()) {
        return std::invoke(std::forward<F>(func), static_cast<T&>(*self.m_handle));
    }
    return std::nullopt;
}

template <typename T>
template <typename Self, typename F>
constexpr auto util::OptionalRef<T>::transform(this Self&& self, F&& func)
{
    if (self.has_value()) {
        using Result = std::invoke_result_t<F, T&>;
        if constexpr (std::is_reference_v<Result>) {
            return OptionalRef{
                std::invoke(std::forward<F>(func), static_cast<T&>(*self.m_handle))
            };
        }
        else {
            return std::optional{
                std::invoke(std::forward<F>(func), static_cast<T&>(*self.m_handle))
            };
        }
    }
    return std::forward<Self>(self);
}

template <typename T>
template <typename Self, util::or_else_func_c F>
constexpr auto util::OptionalRef<T>::or_else(this Self&& self, F&& func)
{
    if (!self.has_value()) {
        return std::invoke(std::forward<F>(func));
    }
    return std::forward<Self>(self);
}
