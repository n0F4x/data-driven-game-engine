module;

#include <cstdint>
#include <type_traits>

export module core.ecs:ID;

import utility.meta.concepts.specialization_of;
import utility.Strong;

import :RegistryTag;

using UnderlyingID = uint64_t;

namespace core::ecs {

export template <typename Registry_T>
using ID = ::util::Strong<::UnderlyingID, void, registry_tag<Registry_T>>;

export template <typename T>
concept specialization_of_id_c =
    util::specialization_of_strong_c<T>
    && util::meta::specialization_of_c<std::remove_const_t<decltype(T::tag)>, RegistryTag>;

}   // namespace core::ecs
