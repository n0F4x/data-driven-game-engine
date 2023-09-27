#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::utils::vulkan {

class ShaderModule {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit ShaderModule(
        vk::Device       t_device,
        vk::ShaderModule t_shader_module
    ) noexcept;
    ShaderModule(ShaderModule&&) noexcept;
    ~ShaderModule() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(ShaderModule&&) noexcept -> ShaderModule& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::ShaderModule;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Device       m_device;
    vk::ShaderModule m_shader_module;
};

}   // namespace engine::utils::vulkan
