export module modules.app.app_c;

import utility.meta.concepts.specialization_of;

import modules.app.App;

namespace modules::app {

export template <typename T>
concept app_c = util::meta::specialization_of_c<T, App>;

}   // namespace app
