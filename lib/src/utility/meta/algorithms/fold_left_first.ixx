module;

#include <functional>

export module utility.meta.algorithms.fold_left_first;

import utility.meta.algorithms.apply;
import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <type_list_c TypeList_T>
    requires(type_list_size_v<TypeList_T> != 0)
constexpr inline auto fold_left_first = []<typename Transform_T, typename Operation_T>(
                                            Transform_T transform,
                                            Operation_T operation
                                        ) static {
    return apply<TypeList_T>([&transform, &operation]<typename... Ts>() {
        return [&transform, &operation]<size_t index_T, typename Accumulated_T>(
                   this auto self, Accumulated_T&& accumulated
               ) {
            if constexpr (index_T == sizeof...(Ts)) {
                return std::forward<Accumulated_T>(accumulated);
            }
            else {
                return self.template operator()<index_T + 1>(std::invoke(
                    operation,
                    std::forward<Accumulated_T>(accumulated),
                    transform.template operator()<Ts...[index_T]>()
                ));
            }
        }.template operator()<1>(transform.template operator()<Ts...[0]>());
    });
};

}   // namespace util::meta
