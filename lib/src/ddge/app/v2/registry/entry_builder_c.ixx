module;

#include <concepts>
#include <type_traits>

export module ddge.app.v2.registry.entry_builder_c;

import ddge.app.v2.registry.BuildableEntryBuilderBase;
import ddge.app.v2.registry.configuration_entry_c;
import ddge.app.v2.registry.entry_c;
import ddge.app.v2.registry.EntryBase;
import ddge.app.v2.registry.EntryBuilderBase;
import ddge.app.v2.registry.represents_entry_dependency_c;
import ddge.util.meta.concepts.storable;

namespace ddge::app::v2 {

template <typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (*)(Dependencies_T...)->Entry_T) -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...)->Entry_T) -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) const->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) &->Entry_T)
    -> bool = delete;

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) const&->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) &&->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

template <typename Builder_T, typename... Dependencies_T, typename Entry_T>
consteval auto is_entry_builder(auto (Builder_T::*)(Dependencies_T...) const&&->Entry_T)
    -> bool
{
    return (represents_entry_dependency_c<Dependencies_T> && ...) && entry_c<Entry_T>;
}

export template <typename T>
concept entry_builder_c = util::meta::storable_c<T>
                       && std::derived_from<T, EntryBuilderBase>
                       && requires { requires is_entry_builder(&T::build); }
                       && (std::default_initializable<T>
                           || std::derived_from<T, internal::BuildableEntryBuilderBase>)
                       && !std::derived_from<T, EntryBase>;

export template <typename T>
concept decays_to_entry_builder_c = entry_builder_c<std::decay_t<T>>;

}   // namespace ddge::app::v2
