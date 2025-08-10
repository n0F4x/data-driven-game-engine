export module ddge.modules.app.app_c;

import ddge.utility.meta.concepts.specialization_of;

import ddge.modules.app.App;

namespace ddge::app {

export template <typename T>
concept app_c = util::meta::specialization_of_c<T, App>;

}   // namespace ddge::app
