#include "StbImage.hpp"

namespace engine::asset {

// auto StbImage::load_from_file(const std::filesystem::path& t_filepath)
//     -> tl::optional<StbImage>
//{
//     int width;
//     int height;
//     int channels_in_file;
//
//     if (!stbi_info(
//             t_filepath.generic_string().c_str(), &width, &height, &channels_in_file
//         ))
//     {
//         return tl::nullopt;
//     }
//
//     stbi_uc* data{ stbi_load(
//         t_filepath.generic_string().c_str(), &width, &height, &channels_in_file,
//         STBI_rgb_alpha
//     ) };
//
//     if (data == nullptr) {
//         SPDLOG_ERROR(stbi_failure_reason());
//         return tl::nullopt;
//     }
//
//     vk::Extent3D extent{ .width  = static_cast<uint32_t>(width),
//                          .height = static_cast<uint32_t>(height),
//                          .depth  = 1u };
//     return ImageInfo{
//         .hash = ImageLoader::hash(t_filepath),
//         .info = StbImage{ .extent = extent, .data = { data, stbi_image_free } }
//     };
// }

}   // namespace engine::asset
