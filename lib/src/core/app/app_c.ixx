export module core.app.app_c;

import utility.meta.concepts.specialization_of;

import core.app.App;

namespace core::app {

export template <typename T>
concept app_c = util::meta::specialization_of_c<T, App>;

}   // namespace core::app
