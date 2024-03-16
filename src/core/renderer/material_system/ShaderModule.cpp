#include "ShaderModule.hpp"

#include <vector>

#include <tl/optional.hpp>

#include "core/utility/vulkan/tools.hpp"

namespace core::renderer {

auto ShaderModule::load(
    vk::Device                   t_device,
    const std::filesystem::path& t_filepath,
    tl::optional<Cache&>         t_cache
) -> tl::optional<Handle<ShaderModule>>
{
    if (auto cached{ t_cache.and_then([&](const Cache& cache) {
            return cache.find<ShaderModule>(std::filesystem::hash_value(t_filepath));
        }) };
        cached.has_value())
    {
        return cached.value();
    }

    vk::UniqueShaderModule module{ vulkan::load_shader(t_device, t_filepath) };
    if (!module) {
        return tl::nullopt;
    }

    return t_cache
        .transform([&](Cache& cache) {
            return cache.emplace<ShaderModule>(
                std::filesystem::hash_value(t_filepath), t_filepath, std::move(module)
            );
        })
        .value_or(make_handle<ShaderModule>(t_filepath, std::move(module)));
}

ShaderModule::ShaderModule(
    std::filesystem::path    t_filepath,
    vk::UniqueShaderModule&& t_module
) noexcept
    : m_filepath{ std::move(t_filepath) },
      m_module{ std::move(t_module) }
{}

auto ShaderModule::filepath() const noexcept -> const std::filesystem::path&
{
    return m_filepath;
}

auto ShaderModule::module() const noexcept -> vk::ShaderModule
{
    return m_module.get();
}

[[nodiscard]] auto hash_value(const ShaderModule& t_shader_module) noexcept -> size_t
{
    return std::filesystem::hash_value(t_shader_module.m_filepath);
}

}   // namespace core::renderer

namespace std {

auto hash<core::renderer::ShaderModule>::operator()(
    const core::renderer::ShaderModule& t_shader_module
) const noexcept -> size_t
{
    return core::renderer::hash_value(t_shader_module);
}

}   // namespace std
