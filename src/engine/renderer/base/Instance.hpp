#pragma once

#include <functional>
#include <span>
#include <string>
#include <vector>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

namespace engine::renderer {

class Instance {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using DebugUtilsMessengerEXTCreator =
        std::function<vk::UniqueDebugUtilsMessengerEXT(vk::Instance)>;

    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct CreateInfo {
        const void*                   next{};
        vk::InstanceCreateFlags       flags{};
        vk::ApplicationInfo           application_info{};
        std::span<const std::string>  layers{};
        std::span<const std::string>  extensions{};
        DebugUtilsMessengerEXTCreator create_debug_messenger{};
    };

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Instance();
    explicit Instance(const CreateInfo& t_create_info);

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator*() const noexcept -> vk::Instance;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Instance*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto get() const noexcept -> vk::Instance;
    [[nodiscard]] auto application_info() const noexcept -> const vk::ApplicationInfo&;
    [[nodiscard]] auto enabled_layers() const noexcept -> std::span<const std::string>;
    [[nodiscard]] auto enabled_extensions() const noexcept
        -> std::span<const std::string>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::ApplicationInfo              m_application_info;
    std::vector<std::string>         m_layers;
    std::vector<std::string>         m_extensions;
    vk::UniqueInstance               m_instance;
    vk::UniqueDebugUtilsMessengerEXT m_debug_utils_messenger;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Instance(const CreateInfo& t_create_info, vk::UniqueInstance&& t_instance);

    explicit Instance(
        const vk::ApplicationInfo&         t_application_info,
        std::span<const std::string>       t_layers,
        std::span<const std::string>       t_extensions,
        vk::UniqueInstance&&               t_instance,
        vk::UniqueDebugUtilsMessengerEXT&& t_debug_utils_messenger
    ) noexcept;
};

}   // namespace engine::renderer
