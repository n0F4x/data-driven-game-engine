module;

#include <type_traits>

export module ddge.utility.meta.type_traits.functional.Signature;

import ddge.utility.meta.concepts.naked;
import ddge.utility.TypeList;

namespace ddge::util::meta {

export template <typename>
struct Signature;

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...)> {
    using type        = Result_T(Args_T...);
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = T;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) noexcept> {
    using type        = Result_T(Args_T...) noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    template <naked_c T>
    using mimic_t = T;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...)&> {
    using type        = Result_T(Args_T...) &;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = T&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) & noexcept> {
    using type        = Result_T(Args_T...) & noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    template <naked_c T>
    using mimic_t = T&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) &&> {
    using type        = Result_T(Args_T...) &&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = T&&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) && noexcept> {
    using type        = Result_T(Args_T...) && noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = T&&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const> {
    using type        = Result_T(Args_T...) const;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = const T;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const noexcept> {
    using type        = Result_T(Args_T...) const noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = const T;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const&> {
    using type        = Result_T(Args_T...) const&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = const T&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const & noexcept> {
    using type        = Result_T(Args_T...) const& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = const T&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const&&> {
    using type        = Result_T(Args_T...) const&&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = const T&&;
};

export template <typename Result_T, typename... Args_T>
struct Signature<Result_T(Args_T...) const && noexcept> {
    using type        = Result_T(Args_T...) const&& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    template <naked_c T>
    using mimic_t = const T&&;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...)> {
    using type        = Result_T (Class_T::*)(Args_T...);
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    using without_class_t = Result_T(Args_T...);
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    using without_class_t = Result_T(Args_T...) noexcept;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...)&> {
    using type        = Result_T (Class_T::*)(Args_T...) &;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    using without_class_t = Result_T(Args_T...) &;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) & noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) & noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    using without_class_t = Result_T(Args_T...) & noexcept;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) &&> {
    using type        = Result_T (Class_T::*)(Args_T...) &&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&&;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    using without_class_t = Result_T(Args_T...) &&;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) && noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) && noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = Class_T&&;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    using without_class_t = Result_T(Args_T...) && noexcept;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const> {
    using type        = Result_T (Class_T::*)(Args_T...) const;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    using without_class_t = Result_T(Args_T...) const;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) const noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    using without_class_t = Result_T(Args_T...) const noexcept;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const&> {
    using type        = Result_T (Class_T::*)(Args_T...) const&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    using without_class_t = Result_T(Args_T...) const&;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const & noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) const& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    using without_class_t = Result_T(Args_T...) const& noexcept;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const&&> {
    using type        = Result_T (Class_T::*)(Args_T...) const&&;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&&;
    constexpr static std::integral_constant<bool, false> is_noexcept;

    using without_class_t = Result_T(Args_T...) const&&;
};

export template <typename Result_T, typename Class_T, typename... Args_T>
struct Signature<Result_T (Class_T::*)(Args_T...) const && noexcept> {
    using type        = Result_T (Class_T::*)(Args_T...) const&& noexcept;
    using arguments_t = TypeList<Args_T...>;
    using result_t    = Result_T;
    using class_t     = const Class_T&&;
    constexpr static std::integral_constant<bool, true> is_noexcept;

    using without_class_t = Result_T(Args_T...) const&& noexcept;
};

}   // namespace ddge::util::meta
