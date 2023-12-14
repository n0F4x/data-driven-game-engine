#pragma once

#include <vulkan/vulkan.hpp>

#include <SFML/System/Vector2.hpp>

namespace engine {

template <typename T>
[[nodiscard]] auto to_extent2D(sf::Vector2<T> t_vector2) noexcept
    -> vk::Extent2D
{
    return vk::Extent2D{ static_cast<uint32_t>(t_vector2.x),
                         static_cast<uint32_t>(t_vector2.y) };
}

}   // namespace engine
