#include "ShaderModule.hpp"

#include <fstream>
#include <optional>
#include <vector>

[[nodiscard]]
static auto load_shader(const vk::Device device, const std::filesystem::path& filepath)
    -> vk::UniqueShaderModule
{
    std::ifstream file{ filepath, std::ios::binary | std::ios::in | std::ios::ate };

    const std::streamsize file_size = file.tellg();
    if (file_size == -1) {
        return {};
    }

    std::vector<char> buffer(static_cast<size_t>(file_size));

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), file_size);
    file.close();

    const vk::ShaderModuleCreateInfo create_info{
        .codeSize = static_cast<size_t>(file_size),
        .pCode    = reinterpret_cast<uint32_t*>(buffer.data())
    };

    return device.createShaderModuleUnique(create_info);
}

namespace core::renderer {

auto ShaderModule::hash(const std::filesystem::path& filepath) noexcept -> size_t
{
    return std::filesystem::hash_value(filepath);
}

auto ShaderModule::create(const vk::Device device, const std::filesystem::path& filepath)
    -> std::optional<ShaderModule>
{
    vk::UniqueShaderModule module{ load_shader(device, filepath) };

    if (!module) {
        return std::nullopt;
    }

    return ShaderModule{ filepath, std::move(module) };
}

ShaderModule::ShaderModule(
    std::filesystem::path    filepath,
    vk::UniqueShaderModule&& module
) noexcept
    : m_filepath{ std::move(filepath) },
      m_module{ std::move(module) }
{}

auto ShaderModule::filepath() const noexcept -> const std::filesystem::path&
{
    return m_filepath;
}

auto ShaderModule::module() const noexcept -> vk::ShaderModule
{
    return m_module.get();
}

[[nodiscard]]
auto hash_value(const ShaderModule& shader_module) noexcept -> size_t
{
    return std::filesystem::hash_value(shader_module.m_filepath);
}

}   // namespace core::renderer

auto std::hash<core::renderer::ShaderModule>::operator()(
    const core::renderer::ShaderModule& shader_module
) const noexcept -> size_t
{
    return core::renderer::hash_value(shader_module);
}
