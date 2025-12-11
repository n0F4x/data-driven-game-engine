export module ddge.modules.vulkan.context;

import vulkan_hpp;

namespace ddge::vulkan {

export [[nodiscard]]
auto context() -> const vk::raii::Context&;

}   // namespace ddge::vulkan

module :private;

auto ddge::vulkan::context() -> const vk::raii::Context&
{
    static const vk::raii::Context static_context;

    return static_context;
}
