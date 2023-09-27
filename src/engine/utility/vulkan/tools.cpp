#include "tools.hpp"

#include <fstream>
#include <set>

namespace engine::utils::vulkan {

auto supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const char* const> t_extensions
) noexcept -> bool
{
    auto [result, extension_properties]{
        t_physical_device.enumerateDeviceExtensionProperties()
    };
    if (result != vk::Result::eSuccess) {
        return false;
    }

    std::set<std::string_view> required_extensions{ t_extensions.begin(),
                                                    t_extensions.end() };

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

auto load_shader(vk::Device t_device, std::string_view t_file_path) noexcept
    -> std::optional<vk::ShaderModule>
{
    std::ifstream file{ t_file_path.data(),
                        std::ios::binary | std::ios::in | std::ios::ate };
    if (!file.is_open()) {
        return std::nullopt;
    }

    std::streamsize file_size = file.tellg();
    if (file_size <= 0) {
        return std::nullopt;
    }

    std::vector<char> buffer(file_size);

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), file_size);
    file.close();

    vk::ShaderModuleCreateInfo create_info{};
    create_info.codeSize = file_size;
    create_info.pCode    = (uint32_t*)buffer.data();

    auto [result, shader_module]{ t_device.createShaderModule(create_info) };
    if (result != vk::Result::eSuccess) {
        return std::nullopt;
    }

    return shader_module;
}

}   // namespace engine::utils::vulkan
