module;

#include <filesystem>

export module ddge.modules.renderer.material_system.Program;

import vulkan_hpp;

import ddge.modules.renderer.material_system.Shader;

namespace ddge::renderer {

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

    friend auto hash_value(const Program& program) noexcept -> std::size_t;
};

}   // namespace ddge::renderer

export template <>
struct std::hash<ddge::renderer::Program> {
    [[nodiscard]]
    auto operator()(const ddge::renderer::Program& program) const noexcept -> std::size_t;
};
