module;

#include <vector>

module core.renderer.base.resources.image_extensions;

import core.renderer.base.resources.Image;

auto core::renderer::base::ext_memory_requirements(const Image& image)
    -> vk::MemoryRequirements
{
    return image.device().getImageMemoryRequirements(image.get());
}

auto core::renderer::base::ext_sparse_memory_requirements(const Image& image)
    -> std::vector<vk::SparseImageMemoryRequirements>
{
    return image.device().getImageSparseMemoryRequirements(image.get());
}
