module;

#include <utility>

export module utility.meta.algorithms.enumerate;

import utility.meta.algorithms.for_each;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <type_list_c TypeList_T>
struct EnumerateClosure {
    template <typename F>
    constexpr static auto operator()(F&& func) -> F
    {
        for_each<std::make_index_sequence<type_list_size_v<TypeList_T>>>(
            [&func]<size_t index_T> {
                func.template operator()<index_T, type_list_at_t<TypeList_T, index_T>>();
            }
        );
        return std::forward<F>(func);
    };
};

export template <type_list_c TypeList_T>
constexpr inline EnumerateClosure<TypeList_T> enumerate;

}   // namespace util::meta
