#pragma once

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace core {

template <unsigned dimensions, typename T>
struct Size;

template <typename T>
struct Size<2, T> {
    T width;
    T height;

    template <typename T2, glm::qualifier Q>
    explicit operator glm::vec<2, T2, Q>() const
    {
        return glm::vec<2, T2, Q>{ static_cast<T2>(width), static_cast<T2>(height) };
    }

    explicit operator vk::Extent2D() const
    {
        return vk::Extent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    }
};

template <typename T>
struct Size<3, T> {
    T width;
    T height;
    T depth;

    template <typename T2, glm::qualifier Q>
    explicit operator glm::vec<3, T2, Q>() const
    {
        return glm::vec<3, T2, Q>{ static_cast<T2>(width),
                                   static_cast<T2>(height),
                                   static_cast<T2>(depth) };
    }

    explicit operator vk::Extent3D() const
    {
        return vk::Extent3D{ static_cast<uint32_t>(width),
                             static_cast<uint32_t>(height),
                             static_cast<uint32_t>(depth) };
    }
};

using Size2i = Size<2, int>;
using Size2f = Size<2, float>;

using Size3i = Size<3, int>;
using Size3f = Size<3, float>;

}   // namespace core