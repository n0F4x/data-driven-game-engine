export module ddge.app.builder_c;

import ddge.util.meta.concepts.specialization_of;

import ddge.app.Builder;

namespace ddge::app {

export template <typename T>
concept builder_c = util::meta::specialization_of_c<T, Builder>;

}   // namespace ddge::app
