#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class Instance {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Instance(vk::Instance t_instance) noexcept;
    Instance(Instance&&) noexcept;
    ~Instance() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Instance&&) noexcept -> Instance&;
    [[nodiscard]] auto operator*() const noexcept -> vk::Instance;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Instance*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto destroy() noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Instance m_instance;
};

}   // namespace engine::vulkan
