module;

#include <cstdint>

export module core.ecs:ID;

import utility.meta.concepts.specialization_of;
import utility.Strong;

namespace core::ecs {

export struct ID : ::util::Strong<uint64_t, ID> {
    using Strong::Strong;
};

}   // namespace core::ecs
