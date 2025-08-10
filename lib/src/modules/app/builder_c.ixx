export module ddge.modules.app.builder_c;

import ddge.utility.meta.concepts.specialization_of;

import ddge.modules.app.Builder;

namespace ddge::app {

export template <typename T>
concept builder_c = util::meta::specialization_of_c<T, Builder>;

}   // namespace ddge::app
