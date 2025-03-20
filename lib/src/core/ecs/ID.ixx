module;

#include <cstdint>
#include <type_traits>

export module core.ecs:ID;

import utility.meta.concepts.specialization_of;
import utility.Strong;

namespace core::ecs {

export using ID = ::util::Strong<uint64_t>;

}   // namespace core::ecs
