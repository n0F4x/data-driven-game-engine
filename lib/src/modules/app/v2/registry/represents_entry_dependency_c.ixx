module;

#include <concepts>
#include <type_traits>

export module ddge.modules.app.v2.registry.represents_entry_dependency_c;

import ddge.modules.app.v2.registry.entry_c;
import ddge.modules.app.v2.registry.EntryBuilderBase;
import ddge.utility.containers.OptionalRef;
import ddge.utility.meta.concepts.specialization_of;

namespace ddge::app::v2 {

export template <typename T>
concept represents_entry_dependency_c =
    (std::is_lvalue_reference_v<T>
     && (entry_c<std::remove_cvref_t<T>>
         || (std::derived_from<std::remove_cvref_t<T>, EntryBuilderBase>
             && std::is_const_v<std::remove_reference_t<T>>)))
    || (util::meta::specialization_of_c<T, util::OptionalRef>
        && (entry_c<std::remove_cvref_t<typename T::ValueType>>
            || (std::derived_from<std::remove_cvref_t<typename T::ValueType>, EntryBuilderBase>
                && std::is_const_v<std::remove_reference_t<typename T::ValueType>>)));

}   // namespace ddge::app::v2
