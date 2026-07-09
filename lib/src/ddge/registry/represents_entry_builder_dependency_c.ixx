module;

#include <concepts>
#include <type_traits>

export module ddge.registry.represents_entry_builder_dependency_c;

import ddge.registry.configuration_entry_c;
import ddge.registry.EntryBuilderBase;
import ddge.util.containers.OptionalRef;
import ddge.util.meta.concepts.specialization_of;

namespace ddge::registry {

export template <typename T>
concept represents_entry_builder_dependency_c =
    (std::is_lvalue_reference_v<T>
     && (std::derived_from<std::remove_cvref_t<T>, EntryBuilderBase>
         || configuration_entry_c<std::remove_cvref_t<T>>))
    || (util::meta::specialization_of_c<T, util::OptionalRef>
        && (std::derived_from<std::remove_cvref_t<typename T::ValueType>, EntryBuilderBase>
            || configuration_entry_c<std::remove_cvref_t<typename T::ValueType>>));

}   // namespace ddge::registry
