module;

#include <cstdint>

export module ddge.ecs:ID;

import ddge.util.meta.concepts.specialization_of;
import ddge.util.containers.Strong;

namespace ddge::ecs {

export struct ID : ddge::util::Strong<uint64_t, ID> {
    using Strong::Strong;
};

}   // namespace ddge::ecs
