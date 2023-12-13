#pragma once

#include <expected>
#include <functional>
#include <span>
#include <string_view>
#include <vector>

#include "engine/utility/vulkan/raii_wrappers.hpp"

namespace engine::renderer {

class Instance {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct CreateInfo {
        const void*                  next{};
        vk::InstanceCreateFlags      flags{};
        vk::ApplicationInfo          application_info{};
        std::span<const std::string> layers{};
        std::span<const std::string> extensions{};
        std::function<vk::DebugUtilsMessengerEXT(vk::Instance)>
            create_debug_messenger{};
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(const CreateInfo& t_extension_name
    ) noexcept -> std::expected<Instance, vk::Result>;

    [[nodiscard]] static auto create_default() noexcept
        -> std::expected<Instance, vk::Result>;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator*() const noexcept -> vk::Instance;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Instance*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto application_info() const noexcept
        -> const vk::ApplicationInfo&;
    [[nodiscard]] auto enabled_layers() const noexcept
        -> std::span<const std::string>;
    [[nodiscard]] auto enabled_extensions() const noexcept
        -> std::span<const std::string>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::ApplicationInfo         m_application_info;
    std::vector<std::string>    m_layers;
    std::vector<std::string>    m_extensions;
    vulkan::Instance            m_instance;
    vulkan::DebugUtilsMessenger m_debug_utils_messenger;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Instance(
        const vk::ApplicationInfo&    t_application_info,
        std::span<const std::string>  t_layers,
        std::span<const std::string>  t_extensions,
        vulkan::Instance&&            t_instance,
        vulkan::DebugUtilsMessenger&& t_debug_utils_messenger
    ) noexcept;
};

}   // namespace engine::renderer
