module;

#include <vector>

module ddge.modules.renderer.base.resources.image_extensions;

import ddge.modules.renderer.base.resources.Image;

auto ddge::renderer::base::ext_memory_requirements(const Image& image)
    -> vk::MemoryRequirements
{
    return image.device().getImageMemoryRequirements(image.get());
}

auto ddge::renderer::base::ext_sparse_memory_requirements(const Image& image)
    -> std::vector<vk::SparseImageMemoryRequirements>
{
    return image.device().getImageSparseMemoryRequirements(image.get());
}
