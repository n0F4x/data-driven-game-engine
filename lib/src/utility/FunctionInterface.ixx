module;

#include <functional>
#include <utility>

export module ddge.utility.FunctionInterface;

import ddge.utility.meta.concepts.type_list.type_list;

namespace ddge::util {

export template <
    typename Result_T,
    util::meta::type_list_c ArgList_T,
    auto                    projection_T,
    auto                    transformation_T = std::identity{}>
class FunctionInterface;

#define NOEXCEPT_RETURN(...) noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

export template <
    typename Result_T,
    template <typename...> typename TypeList_T,
    typename... Args_T,
    auto projection_T,
    auto transformation_T>
class FunctionInterface<Result_T, TypeList_T<Args_T...>, projection_T, transformation_T> {
public:
    // ReSharper disable once CppEnforceFunctionDeclarationStyle
    template <typename Self_T>
    constexpr Result_T operator()(this Self_T&& self, Args_T... args) NOEXCEPT_RETURN(
        std::invoke(
            transformation_T,
            std::invoke(
                std::invoke(projection_T, std::forward<Self_T>(self)),
                std::forward<Args_T>(args)...
            )
        )
    )
};

}   // namespace ddge::util
