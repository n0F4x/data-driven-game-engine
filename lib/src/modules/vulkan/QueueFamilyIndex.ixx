module;

#include <cstdint>

export module ddge.modules.vulkan.QueueFamilyIndex;

import ddge.utility.containers.Strong;

namespace ddge::vulkan {

export class QueueFamilyIndex : public util::Strong<uint32_t, QueueFamilyIndex> {
public:
    using Strong::Strong;
};

}   // namespace ddge::vulkan
