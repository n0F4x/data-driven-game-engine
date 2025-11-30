module;

module ddge.deprecated.image.Image;

import vulkan_hpp;

auto ddge::image::Image::extent() const noexcept -> vk::Extent3D
{
    return vk::Extent3D{
        .width  = width(),
        .height = height(),
        .depth  = depth(),
    };
}
