module;

#include <cstdint>

export module modules.ecs:ID;

import utility.meta.concepts.specialization_of;
import utility.Strong;

namespace modules::ecs {

export struct ID : ::util::Strong<uint64_t, ID> {
    using Strong::Strong;
};

}   // namespace modules::ecs
