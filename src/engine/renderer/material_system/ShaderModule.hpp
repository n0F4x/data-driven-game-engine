#pragma once

#include <filesystem>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "engine/common/Cache.hpp"
#include "engine/common/Handle.hpp"

#include "VertexAttribute.hpp"

namespace engine::renderer {

class ShaderModule {
public:
    using AttributeLocations =
        std::array<tl::optional<uint32_t>, static_cast<size_t>(VertexAttribute::COUNT)>;

    [[nodiscard]] static auto load(
        vk::Device                   t_device,
        const std::filesystem::path& t_filepath,
        tl::optional<Cache&>         t_cache = {}
    ) -> tl::optional<Handle<ShaderModule>>;

    explicit ShaderModule(
        std::filesystem::path    t_filepath,
        vk::UniqueShaderModule&& t_module
    ) noexcept;

    [[nodiscard]] auto attribute_locations() const noexcept -> const AttributeLocations&;
    [[nodiscard]] auto filepath() const noexcept -> const std::filesystem::path&;
    [[nodiscard]] auto module() const noexcept -> vk::ShaderModule;

private:
    AttributeLocations     m_attributes;
    std::filesystem::path  m_filepath;
    vk::UniqueShaderModule m_module;

    friend auto hash_value(const ShaderModule& t_shader_module) noexcept -> size_t;
};

}   // namespace engine::renderer

namespace std {

template <>
class hash<engine::renderer::ShaderModule> {
public:
    [[nodiscard]] auto operator()(const engine::renderer::ShaderModule& t_shader_module
    ) const noexcept -> size_t;
};

}   // namespace std
