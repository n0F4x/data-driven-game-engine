module;

#include <vector>

module modules.renderer.base.resources.image_extensions;

import modules.renderer.base.resources.Image;

auto modules::renderer::base::ext_memory_requirements(const Image& image)
    -> vk::MemoryRequirements
{
    return image.device().getImageMemoryRequirements(image.get());
}

auto modules::renderer::base::ext_sparse_memory_requirements(const Image& image)
    -> std::vector<vk::SparseImageMemoryRequirements>
{
    return image.device().getImageSparseMemoryRequirements(image.get());
}
