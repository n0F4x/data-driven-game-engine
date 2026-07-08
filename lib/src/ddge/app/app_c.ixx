export module ddge.app.app_c;

import ddge.util.meta.concepts.specialization_of;

import ddge.app.App;

namespace ddge::app {

export template <typename T>
concept app_c = util::meta::specialization_of_c<T, App>;

}   // namespace ddge::app
