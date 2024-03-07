#pragma once

#include <functional>
#include <memory>
#include <variant>

#include <vulkan/vulkan.hpp>

#include <stb_image.h>

#include <ktx.h>
#include <ktxvulkan.h>

namespace engine::renderer {

struct StbImage {
    vk::Extent3D                                         extent;
    std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data;
};

using KtxImageInfo = std::unique_ptr<ktxTexture, std::function<void(ktxTexture*)>>;

struct ImageInfo {
    entt::id_type                        hash;
    std::variant<StbImage, KtxImageInfo> info;
};

}   // namespace engine::renderer
