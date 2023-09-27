#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::utils::vulkan {

class Surface {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Surface(
        vk::Instance   t_instance,
        vk::SurfaceKHR t_surface
    ) noexcept;
    Surface(Surface&&) noexcept;
    ~Surface() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Surface&&) noexcept -> Surface& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::SurfaceKHR;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Instance   m_instance;
    vk::SurfaceKHR m_surface;
};

}   // namespace engine::utils::vulkan
