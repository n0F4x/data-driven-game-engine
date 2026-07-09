module;

#include <concepts>

export module ddge.registry.BuildableEntryBuilder;

import ddge.registry.BuildableEntryBuilderBase;
import ddge.registry.BuildDirector;
import ddge.registry.EntryBuilderBase;

namespace ddge::registry {

template <typename T, typename EntryBuilder_T>
concept build_description_c = std::invocable<T, BuildDirector<EntryBuilder_T>&>;

export template <typename EntryBuilder_T, auto describe_build_T>
class BuildableEntryBuilder : public internal::BuildableEntryBuilderBase {
    friend auto describe_build(BuildDirector<EntryBuilder_T>& build_director) -> void
    {
        static_assert(build_description_c<decltype(describe_build_T), EntryBuilder_T>);
        static_assert(std::derived_from<EntryBuilder_T, BuildableEntryBuilder>);
        std::invoke(describe_build_T, build_director);
    }
};

}   // namespace ddge::registry
