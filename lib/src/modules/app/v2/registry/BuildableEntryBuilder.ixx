module;

#include <concepts>

export module ddge.modules.app.v2.registry.BuildableEntryBuilder;

import ddge.modules.app.v2.registry.BuildableEntryBuilderBase;
import ddge.modules.app.v2.registry.BuildDirector;
import ddge.modules.app.v2.registry.EntryBuilderBase;

namespace ddge::app::v2 {

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

}   // namespace ddge::app::v2
