// ReSharper disable once CppMissingIncludeGuard

#ifdef ENGINE_TEMP_INVOKE_QUALS
  #error ENGINE_TEMP_INVOKE_QUALS should not be defined
#endif
#ifdef ENGINE_TEMP_CONST_REF
  #error ENGINE_TEMP_CONST_REF should not be defined
#endif

#ifndef ENGINE_TEMP_CONST
  #define ENGINE_TEMP_CONST
#endif
#ifndef ENGINE_TEMP_REF
  #define ENGINE_TEMP_REF
  #define ENGINE_TEMP_INVOKE_QUALS ENGINE_TEMP_CONST&
#else
  #define ENGINE_TEMP_INVOKE_QUALS ENGINE_TEMP_CONST ENGINE_TEMP_REF
#endif

#define ENGINE_TEMP_CONST_REF ENGINE_TEMP_CONST ENGINE_TEMP_REF

namespace ENGINE_TEMP_NAMESPACE {

template <typename T, typename Result_T, typename... Args_T>
constexpr static bool callable_with_noexcept_v = std::conditional_t<
    ENGINE_TEMP_IS_NOEXCEPT,
    std::is_nothrow_invocable_r<Result_T, T, Args_T...>,
    std::is_invocable_r<Result_T, T, Args_T...>>::value;

template <typename T, typename Result_T, typename... Args_T>
constexpr static bool callable_with_qualifiers_v =
    callable_with_noexcept_v<T ENGINE_TEMP_INVOKE_QUALS, Result_T, Args_T...>
    && callable_with_noexcept_v<T ENGINE_TEMP_CONST_REF, Result_T, Args_T...>;

template <std::size_t size_T, std::size_t alignment_T, typename Result_T, typename... Args_T>
class Invoker {
    template <typename T>
    struct Dispatch {
        constexpr static auto operator()(
            ENGINE_TEMP_CONST internal::Storage<size_T, alignment_T>& storage,
            Args_T... args
        ) noexcept(ENGINE_TEMP_IS_NOEXCEPT) -> Result_T
        {
            if constexpr (uses_small_buffer_optimization_c<T, size_T, alignment_T>) {
                return std::invoke_r<Result_T>(
                    static_cast<T ENGINE_TEMP_INVOKE_QUALS>(*storage.template launder<T>()),
                    std::forward<Args_T>(args)...
                );
            }
            else {
                T* handle{ *storage.template launder<T*>() };
                PRECOND(handle != nullptr, "Do not call a moved-from `Function`!");
                return std::invoke_r<Result_T>(
                    static_cast<T ENGINE_TEMP_INVOKE_QUALS>(*handle),
                    std::forward<Args_T>(args)...
                );
            }
        }
    };

public:
    template <typename T>
    constexpr explicit Invoker(std::type_identity<T>)
        : m_invoke{ Dispatch<T>::operator() }
    {}

    constexpr auto operator()(
        ENGINE_TEMP_CONST internal::Storage<size_T, alignment_T>& storage,
        Args_T... args
    ) ENGINE_TEMP_CONST noexcept(ENGINE_TEMP_IS_NOEXCEPT) -> Result_T
    {
        return m_invoke(storage, std::forward<Args_T>(args)...);
    }

private:
    using InvokeFunc =
        auto(ENGINE_TEMP_CONST internal::Storage<size_T, alignment_T>&, Args_T...)
            -> Result_T;

    std::reference_wrapper<InvokeFunc> m_invoke;
};

}   // namespace ENGINE_TEMP_NAMESPACE

export template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
class Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T> : public internal::FunctionBase   //
{
public:
    using Signature = auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(
        ENGINE_TEMP_IS_NOEXCEPT
    ) -> Result_T;
    using allocator_type = std::pmr::polymorphic_allocator<>;


    [[nodiscard]]
    consteval static auto is_move_only() -> bool;
    [[nodiscard]]
    consteval static auto size() -> std::size_t;
    [[nodiscard]]
    consteval static auto alignment() -> std::size_t;


    constexpr Function(const Function&, const allocator_type& allocator = {})
        requires(!is_move_only_T);
    constexpr Function(Function&&) noexcept;
    constexpr Function(Function&&, const allocator_type& allocator);
    constexpr ~Function();

    template <meta::decayed_c T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit Function(std::in_place_type_t<T>, Args_T&&... args)
        requires storable_in_function_c<T, Function>;

    template <meta::decayed_c T, typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    constexpr explicit Function(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<T>,
        Args_T&&... args
    )
        requires storable_in_function_c<T, Function>;

    template <typename T>
    constexpr explicit Function(T&& value)
        requires(!meta::strips_to_c<T, Function>)
             && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && storable_in_function_c<std::decay_t<T>, Function>;

    template <typename T>
    constexpr explicit Function(
        std::allocator_arg_t,
        const allocator_type& allocator,
        T&&                   value
    )
        requires(!meta::strips_to_c<T, Function>)
             && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
             && std::constructible_from<std::decay_t<T>, T&&>
             && storable_in_function_c<std::decay_t<T>, Function>;


    constexpr auto operator=(const Function&) -> Function&
        requires(!is_move_only_T);
    constexpr auto operator=(Function&&) noexcept -> Function&
        requires is_move_only_T;
    constexpr auto operator=(Function&&) noexcept -> Function&
        requires(!is_move_only_T);


    [[nodiscard]]
    constexpr auto get_allocator() const noexcept -> allocator_type;


    constexpr auto operator()(FArgs_T... args) ENGINE_TEMP_CONST_REF noexcept(
        ENGINE_TEMP_IS_NOEXCEPT
    ) -> Result_T;


    // ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
    // NOLINTNEXTLINE(*-noexcept-swap)
    constexpr auto swap(Function& other) -> void;


    template <meta::decayed_c T, typename Function_T>
        requires storable_in_function_c<T, std::remove_cvref_t<Function_T>>
    friend constexpr auto any_cast(Function_T&& function)
        -> meta::forward_like_t<T, Function_T>;

    template <meta::decayed_c T, typename Function_T>
        requires std::derived_from<std::remove_cvref_t<Function_T>, FunctionBase>
    friend constexpr auto reinterpret_any_cast(Function_T&& function)
        -> meta::forward_like_t<T, Function_T>;

private:
    allocator_type                                               m_allocator;
    LifeCycleEraseMechanism<is_move_only_T, size_T, alignment_T> m_erase_mechanism;
    ENGINE_TEMP_NAMESPACE::Invoker<size_T, alignment_T, Result_T, FArgs_T...> m_invoke;
    internal::Storage<size_T, alignment_T>                                    m_storage;


    constexpr auto reset() -> void;
};

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
consteval auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::is_move_only() -> bool
{
    return is_move_only_T;
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
consteval auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::size() -> std::size_t
{
    return size_T;
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
consteval auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::alignment() -> std::size_t
{
    return alignment_T;
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::Function(const Function& other, const allocator_type& allocator)
    requires(!is_move_only_T)
    : m_allocator{ allocator },
      m_erase_mechanism{ other.m_erase_mechanism },
      m_invoke{ other.m_invoke },
      m_storage{ m_erase_mechanism.copy_construct(m_allocator, other.m_storage) }
{}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::Function(Function&& other) noexcept
    : m_allocator{ other.m_allocator },
      m_erase_mechanism{ other.m_erase_mechanism },
      m_invoke{ other.m_invoke },
      m_storage{ m_erase_mechanism.move_construct(std::move(other.m_storage)) }
{}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::Function(Function&& other, const allocator_type& allocator)
    : m_allocator{ allocator },
      m_erase_mechanism{ other.m_erase_mechanism },
      m_invoke{ other.m_invoke },
      m_storage{
          m_erase_mechanism
              .move_construct(m_allocator, allocator, std::move(other.m_storage)),
      }
{}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::~Function()
{
    m_erase_mechanism.drop(m_allocator, m_storage);
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <meta::decayed_c T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::Function(std::in_place_type_t<T> in_place_type, Args_T&&... args)
    requires storable_in_function_c<T, Function>
    : Function{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          in_place_type,
          std::forward<Args_T>(args)...,
      }
{}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <meta::decayed_c T, typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::
    Function(
        std::allocator_arg_t,
        const allocator_type&         allocator,
        const std::in_place_type_t<T> in_place_type,
        Args_T&&... args
    )
    requires storable_in_function_c<T, Function>
    : m_allocator{ allocator },
      m_erase_mechanism{ std::type_identity<T>{} },
      m_invoke{ std::type_identity<T>{} },
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
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <typename T>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::Function(T&& value)
    requires(!meta::strips_to_c<T, Function>)
         && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
         && storable_in_function_c<std::decay_t<T>, Function>
    : Function{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          std::forward<T>(value),
      }
{}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
template <typename T>
constexpr Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::Function(std::allocator_arg_t, const allocator_type& allocator, T&& value)
    requires(!meta::strips_to_c<T, Function>)
         && (!meta::specialization_of_c<std::remove_cvref_t<T>, std::in_place_type_t>)
         && std::constructible_from<std::decay_t<T>, T&&>
         && storable_in_function_c<std::decay_t<T>, Function>
    : Function{
          std::allocator_arg,
          allocator,
          std::in_place_type<std::decay_t<T>>,
          std::forward<T>(value),
      }
{}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::operator=(const Function& other) -> Function&
    requires(!is_move_only_T)
{
    if (this == &other) {
        return *this;
    }

    other.m_erase_mechanism.copy_assign(
        m_allocator, m_storage, m_erase_mechanism, other.m_storage
    );
    m_erase_mechanism = other.m_erase_mechanism;
    m_invoke          = other.m_invoke;

    return *this;
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::operator=(Function&& other) noexcept -> Function&
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
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::operator=(Function&& other) noexcept -> Function&
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
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::get_allocator() const noexcept -> allocator_type
{
    return m_allocator;
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::
    operator()(FArgs_T... args) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)
        -> Result_T
{
    return m_invoke(m_storage, std::forward<FArgs_T>(args)...);
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::swap(Function& other) -> void
{
    m_erase_mechanism.swap(
        m_allocator, m_storage, other.m_allocator, other.m_storage, other.m_erase_mechanism
    );
    std::swap(m_erase_mechanism, other.m_erase_mechanism);
    std::swap(m_invoke, other.m_invoke);
}

template <
    typename Result_T,
    typename... FArgs_T,
    bool        is_move_only_T,
    std::size_t size_T,
    std::size_t alignment_T>
constexpr auto Function<
    auto(FArgs_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T,
    is_move_only_T,
    size_T,
    alignment_T>::reset() -> void
{
    m_erase_mechanism.reset(m_allocator, m_storage);
}

template <typename T, typename Result_T, typename... Args_T>
constexpr bool internal::callable_with_qualifiers_v<
    T,
    auto(Args_T...) ENGINE_TEMP_CONST_REF noexcept(ENGINE_TEMP_IS_NOEXCEPT)->Result_T> =
    ENGINE_TEMP_NAMESPACE::callable_with_qualifiers_v<T, Result_T, Args_T...>;

#undef ENGINE_TEMP_NAMESPACE
#undef ENGINE_TEMP_CONST
#undef ENGINE_TEMP_REF
#undef ENGINE_TEMP_IS_NOEXCEPT
#undef ENGINE_TEMP_INVOKE_QUALS
#undef ENGINE_TEMP_CONST_REF
