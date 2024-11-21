module;

#include <filesystem>

#include <vulkan/vulkan.hpp>

export module core.renderer.material_system.Program;

import core.renderer.material_system.Shader;

namespace core::renderer {

export class Program {
public:
    explicit Program(Shader vertex_shader, Shader fragment_shader) noexcept;

    [[nodiscard]]
    auto vertex_shader() const noexcept -> const Shader&;
    [[nodiscard]]
    auto fragment_shader() const noexcept -> const Shader&;

    [[nodiscard]]
    auto pipeline_stages() const -> std::array<vk::PipelineShaderStageCreateInfo, 2>;

private:
    Shader m_vertex_shader;
    Shader m_fragment_shader;

    friend auto hash_value(const Program& program) noexcept -> size_t;
};

}   // namespace core::renderer

export template <>
struct std::hash<core::renderer::Program> {
    [[nodiscard]]
    auto operator()(const core::renderer::Program& program) const noexcept -> size_t;
};
