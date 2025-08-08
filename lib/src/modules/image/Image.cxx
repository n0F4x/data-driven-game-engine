module;

module modules.image.Image;

import vulkan_hpp;

auto modules::image::Image::extent() const noexcept -> vk::Extent3D
{
    return vk::Extent3D{
        .width  = width(),
        .height = height(),
        .depth  = depth(),
    };
}
