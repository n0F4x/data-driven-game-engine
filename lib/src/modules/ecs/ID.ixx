module;

#include <cstdint>

export module ddge.modules.ecs:ID;

import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.containers.Strong;

namespace ddge::ecs {

export struct ID : ddge::util::Strong<uint64_t, ID> {
    using Strong::Strong;
};

}   // namespace ddge::ecs
