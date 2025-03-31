export module utility.meta.algorithms.any_of;

import utility.meta.algorithms.apply;
import utility.meta.concepts.type_list.type_list;

namespace util::meta {

export template <meta::type_list_c TypeList_T>
constexpr inline auto any_of =
    []<typename Predicate_T>(Predicate_T&& predicate) static -> bool {
    return apply<TypeList_T>([&predicate]<typename... Ts> -> bool {
        return (predicate.template operator()<Ts>() || ...);
    });
};

}   // namespace util::meta
