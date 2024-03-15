#include "Effect.hpp"

#include <string_view>
#include <vector>

#include <spirv_cross.hpp>

#include "engine/utility/hashing.hpp"

namespace engine::renderer {

[[nodiscard]] static auto load_spirv_from_file(const std::filesystem::path& t_filepath)
    -> std::vector<uint32_t>
{
    std::ifstream file{ t_filepath, std::ios::binary | std::ios::in | std::ios::ate };

    const std::streamsize file_size = file.tellg();
    if (file_size == -1) {
        return {};
    }

    std::vector<uint32_t> buffer(static_cast<size_t>(file_size));

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    file.close();

    return buffer;
}

[[nodiscard]] static auto match_attribute_name(const std::string_view t_name) noexcept
    -> tl::optional<VertexAttribute>
{
    std::string lowercase_name{ t_name };
    std::ranges::transform(lowercase_name, lowercase_name.begin(), ::tolower);

    if (lowercase_name.contains("position")) {
        return VertexAttribute::ePosition;
    }
    if (lowercase_name.contains("normal")) {
        return VertexAttribute::eNormal;
    }
    if (lowercase_name.contains("tangent")) {
        return VertexAttribute::eTangent;
    }
    if (lowercase_name.contains("color")) {
        return VertexAttribute::eColor;
    }
    if (lowercase_name.contains("uv") || lowercase_name.contains("tex")
        || lowercase_name.contains("coord") || lowercase_name.contains("texcord"))
    {
        if (lowercase_name.contains("1")) {
            return VertexAttribute::eTexCoord_1;
        }
        else {
            return VertexAttribute::eTexCoord_0;
        }
    }

    return tl::nullopt;
}

auto Effect::InputAttributeLocations::load_from_shader_file(
    const std::filesystem::path& t_filepath
) -> InputAttributeLocations
{
    std::vector<uint32_t>        spirv = load_spirv_from_file(t_filepath);
    spirv_cross::Compiler        compiler{ std::move(spirv) };
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    InputAttributeLocations locations;

    for (const auto& input : resources.stage_inputs) {
        if (const auto attribute{ match_attribute_name(input.name) };
            attribute.has_value())
        {
            locations
                .m_locations[static_cast<size_t>(std::to_underlying(attribute.value()))] =
                compiler.get_decoration(input.id, spv::DecorationLocation);
        }
    }

    return locations;
}

auto Effect::InputAttributeLocations::get(const VertexAttribute t_attribute
) const noexcept -> tl::optional<uint32_t>
{
    return m_locations[static_cast<size_t>(std::to_underlying(t_attribute))];
}

Effect::Effect(Handle<Shader> t_vertex_shader, Handle<Shader> t_fragment_shader) noexcept
    : m_vertex_shader{ std::move(t_vertex_shader) },
      m_fragment_shader{ std::move(t_fragment_shader) },
      m_stages{ vk::PipelineShaderStageCreateInfo{   .stage  = vk::ShaderStageFlagBits::eVertex,
                                                   .module = m_vertex_shader->module(),
                                                   .pName  = m_vertex_shader->entry_point().c_str() },
                vk::PipelineShaderStageCreateInfo{ .stage  = vk::ShaderStageFlagBits::eFragment,
                                                   .module = m_fragment_shader->module(),
                                                   .pName  = m_fragment_shader->entry_point().c_str() } },
      m_attribute_locations{ InputAttributeLocations::load_from_shader_file(m_vertex_shader->filepath()) }
{}

auto Effect::vertex_shader() const noexcept -> const Shader&
{
    return *m_vertex_shader;
}

auto Effect::fragment_shader() const noexcept -> const Shader&
{
    return *m_fragment_shader;
}

auto Effect::pipeline_stages() const -> std::span<const vk::PipelineShaderStageCreateInfo>
{
    return m_stages;
}

[[nodiscard]] auto hash_value(const Effect& t_effect) noexcept -> size_t
{
    return hash_combine(*t_effect.m_vertex_shader, *t_effect.m_fragment_shader);
}

}   // namespace engine::renderer

namespace std {

auto hash<engine::renderer::Effect>::operator()(const engine::renderer::Effect& t_effect
) const -> size_t
{
    return engine::renderer::hash_value(t_effect);
}

}   // namespace std
