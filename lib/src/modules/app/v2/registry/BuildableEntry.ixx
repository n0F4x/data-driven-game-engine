module;

#include <concepts>
#include <functional>

export module ddge.modules.app.v2.registry.BuildableEntry;

import ddge.modules.app.v2.registry.BuildableEntryBase;
import ddge.modules.app.v2.registry.BuildDirector;

namespace ddge::app::v2 {

template <typename T, typename Entry_T>
concept build_description_c = std::invocable<T, BuildDirector<Entry_T>&>;

export template <typename Entry_T, auto describe_build_T>
class BuildableEntry : public internal::BuildableEntryBase {
    friend auto describe_build(BuildDirector<Entry_T>& build_director) -> void
    {
        static_assert(build_description_c<decltype(describe_build_T), Entry_T>);
        static_assert(std::derived_from<Entry_T, BuildableEntry>);
        std::invoke(describe_build_T, build_director);
    }
};

}   // namespace ddge::app::v2
