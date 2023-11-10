#pragma once

#include <expected>
#include <functional>
#include <span>
#include <vector>

#include "engine/utility/vulkan/Instance.hpp"

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
        std::span<const char* const> layers{};
        std::span<const char* const> extensions{};
        std::function<vk::DebugUtilsMessengerEXT(vk::Instance)>
            create_debug_messenger{};
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(const CreateInfo& t_create_info) noexcept
        -> std::expected<Instance, vk::Result>;

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
        -> std::span<const char* const>;
    [[nodiscard]] auto enabled_extensions() const noexcept
        -> std::span<const char* const>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::ApplicationInfo      m_application_info;
    std::vector<const char*> m_layers;
    std::vector<const char*> m_extensions;
    vulkan::Instance         m_instance;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Instance(
        const vk::ApplicationInfo&   t_application_info,
        std::span<const char* const> t_layers,
        std::span<const char* const> t_extensions,
        vulkan::Instance&&           t_instance
    ) noexcept;
};

}   // namespace engine::renderer
