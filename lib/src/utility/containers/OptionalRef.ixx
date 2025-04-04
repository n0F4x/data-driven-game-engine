module;

#include <cassert>
#include <memory>
#include <optional>
#include <type_traits>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <expected>
#endif

export module utility.containers.OptionalRef;

namespace util {

export template <typename T>
    requires(!std::is_reference_v<T>)
class OptionalRef;

}   // namespace util

template <typename F, typename T>
concept and_then_func_c =
    std::constructible_from<std::invoke_result_t<F, T&>, std::nullopt_t>;

template <typename F, typename T>
using transform_result_t = std::conditional_t<
    std::is_reference_v<std::invoke_result_t<F, T&>>,
    util::OptionalRef<std::remove_reference_t<std::invoke_result_t<F, T&>>>,
    std::optional<std::invoke_result_t<F, T&>>>;

template <typename F, typename T>
concept transform_func_c = std::invocable<F, T&>;

template <typename F, typename T>
concept or_else_func_c = std::constructible_from<std::invoke_result_t<F>, T&>;

namespace util {

export template <typename T>
    requires(!std::is_reference_v<T>)
class OptionalRef {
public:
    OptionalRef() = default;
    constexpr explicit(false) OptionalRef(std::nullopt_t) noexcept;
    constexpr explicit(false
    ) OptionalRef(OptionalRef<std::remove_const_t<T>> other) noexcept
        requires(std::is_const_v<T>);
    constexpr explicit OptionalRef(T& ref) noexcept;
    constexpr explicit(false) OptionalRef(
        const std::optional<std::reference_wrapper<T>>& optional_ref_wrapper
    ) noexcept;

    [[nodiscard]]
    constexpr auto operator->() const -> T*;
    [[nodiscard]]
    constexpr auto operator*() const -> T&;

    [[nodiscard]]
    constexpr auto has_value() const noexcept -> bool;

    [[nodiscard]]
    constexpr auto value_or(T& other) const noexcept -> T&;

    template <and_then_func_c<T> F>
    constexpr auto and_then(F&& func) const -> std::invoke_result_t<F, T>;
    template <transform_func_c<T> F>
    constexpr auto transform(F&& func) const -> transform_result_t<F, T>;
    template <or_else_func_c<T> F>
    constexpr auto or_else(F&& func) const -> std::invoke_result_t<F>;

private:
    T* m_handle{};
};

}   // namespace util

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(std::nullopt_t) noexcept : OptionalRef{}
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(
    OptionalRef<std::remove_const_t<T>> other
) noexcept
    requires(std::is_const_v<T>)
    : m_handle{ other.transform([](T& value) { return std::addressof(value); }
      ).value_or(nullptr) }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(T& ref) noexcept
    : m_handle{ std::addressof(ref) }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(
    const std::optional<std::reference_wrapper<T>>& optional_ref_wrapper
) noexcept
    : m_handle{ optional_ref_wrapper.transform([](T& ref) { return std::addressof(ref); }
      ).value_or(nullptr) }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::operator->() const -> T*
{
    assert(m_handle != nullptr);
    return m_handle;
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::operator*() const -> T&
{
    assert(m_handle != nullptr);
    return *m_handle;
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::has_value() const noexcept -> bool
{
    return m_handle != nullptr;
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::value_or(T& other) const noexcept -> T&
{
    if (has_value()) {
        return *m_handle;
    }
    return other;
}

template <typename T>
    requires(!std::is_reference_v<T>)
template <and_then_func_c<T> F>
constexpr auto util::OptionalRef<T>::and_then(F&& func) const
    -> std::invoke_result_t<F, T>
{
    if (has_value()) {
        return std::invoke(std::forward<F>(func), static_cast<T&>(*m_handle));
    }
    return std::nullopt;
}

template <typename T>
    requires(!std::is_reference_v<T>)
template <transform_func_c<T> F>
constexpr auto util::OptionalRef<T>::transform(F&& func) const -> transform_result_t<F, T>
{
    if (has_value()) {
        return transform_result_t<F, T>{
            std::invoke(std::forward<F>(func), static_cast<T&>(*m_handle))
        };
    }
    return std::nullopt;
}

template <typename T>
    requires(!std::is_reference_v<T>)
template <or_else_func_c<T> F>
constexpr auto util::OptionalRef<T>::or_else(F&& func) const -> std::invoke_result_t<F>
{
    if (!has_value()) {
        return std::invoke(std::forward<F>(func));
    }
    return { **this };
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional_ref{};
        [[maybe_unused]]
        auto optional{ optional_ref.transform([](int&) -> float { return {}; }) };

        static_assert(std::is_same_v<decltype(optional), std::optional<float>>);

        return true;
    }(),
    "transform to optional test failed"
);

static_assert(
    [] {
        int                          value{ 2 };
        const util::OptionalRef<int> optional_ref{ value };
        constexpr static float       other_value{ 3.2f };
        const auto                   optional{ optional_ref.transform([](int&) -> float {
            return other_value;
        }) };

        assert(optional.value() == other_value);

        return true;
    }(),
    "transform to optional with value test failed"
);

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional_ref{};
        constexpr static float           other_value{ 3.2f };
        constexpr auto optional{ optional_ref.transform([](int&) -> float {
            return other_value;
        }) };

        assert(!optional.has_value());

        return true;
    }(),
    "transform to optional without value test failed"
);

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional{};
        [[maybe_unused]]
        auto expected{ optional.or_else([] -> std::expected<int, float> { return {}; }) };

        static_assert(std::is_same_v<decltype(expected), std::expected<int, float>>);

        return true;
    }(),
    "or_else to expected test failed"
);

static_assert(
    [] {
        int                          value{ 2 };
        const util::OptionalRef<int> optional{ value };
        const auto expected{ optional.or_else([] -> std::expected<int, float> {
            return {};
        }) };

        assert(expected.value() == value);

        return true;
    }(),
    "or_else with value test failed"
);

static_assert(
    [] {
        constexpr util::OptionalRef<int> optional{};
        constexpr static float           error{ 6.7f };
        constexpr auto expected{ optional.or_else([] -> std::expected<int, float> {
            return std::unexpected<float>{ error };
        }) };

        assert(expected.error() == error);

        return true;
    }(),
    "or_else without value test failed"
);

#endif
