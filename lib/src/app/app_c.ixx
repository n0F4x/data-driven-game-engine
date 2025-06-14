export module app.app_c;

import utility.meta.concepts.specialization_of;

import app.App;

namespace app {

export template <typename T>
concept app_c = util::meta::specialization_of_c<T, App>;

}   // namespace app
