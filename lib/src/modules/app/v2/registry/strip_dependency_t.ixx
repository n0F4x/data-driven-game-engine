module;

#include <type_traits>

export module ddge.modules.app.v2.registry.strip_dependency_t;

import ddge.util.containers.OptionalRef;
import ddge.util.meta.concepts.specialization_of;

namespace ddge::app::v2 {

template <typename T>
struct StripDependency {
    using type = std::remove_cvref_t<T>;
};

template <typename T>
struct StripDependency<util::OptionalRef<T>> {
    using type = std::remove_const_t<T>;
};

export template <typename T>
using strip_dependency_t = StripDependency<T>::type;

}   // namespace ddge::app::v2
