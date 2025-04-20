export module core.app.builder_c;

import utility.meta.concepts.specialization_of;

import core.app.Builder;

namespace core::app {

export template <typename T>
concept builder_c = util::meta::specialization_of_c<T, Builder>;

}   // namespace core::app
