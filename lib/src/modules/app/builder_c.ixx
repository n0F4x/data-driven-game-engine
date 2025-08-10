export module modules.app.builder_c;

import utility.meta.concepts.specialization_of;

import modules.app.Builder;

namespace modules::app {

export template <typename T>
concept builder_c = util::meta::specialization_of_c<T, Builder>;

}   // namespace app
