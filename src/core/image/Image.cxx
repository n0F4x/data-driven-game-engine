module;



module core.image.Image;

auto core::image::Image::extent() const noexcept -> vk::Extent3D
{
    return vk::Extent3D{
        .width  = width(),
        .height = height(),
        .depth  = depth(),
    };
}
