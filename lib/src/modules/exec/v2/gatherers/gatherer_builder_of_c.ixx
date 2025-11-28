module;

#include <memory>
#include <utility>

export module ddge.modules.exec.v2.gatherers.gatherer_builder_of_c;

import ddge.modules.exec.v2.gatherers.gatherer_of_c;

import ddge.utility.meta.concepts.specialization_of;

namespace ddge::exec::v2 {

template <typename T, typename Input_T>
concept shared_gatherer = util::meta::specialization_of_c<T, std::shared_ptr>
                       && gatherer_of_c<typename T::element_type, Input_T>;

export template <typename T, typename Input_T>
concept gatherer_builder_of_c = requires {
    typename T::Result;
} && requires(T&& gatherer_builder, const uint32_t number_of_gathers) {
    { std::move(gatherer_builder).build(number_of_gathers) } -> shared_gatherer<Input_T>;
};

}   // namespace ddge::exec::v2
