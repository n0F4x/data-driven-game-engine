#include "Node.hpp"

#include <glm/gtc/quaternion.hpp>

#include "Model.hpp"

namespace core::gltf {

auto Node::translation() const noexcept -> const glm::vec3&
{
    return m_translation;
}

auto Node::translation() noexcept -> glm::vec3&
{
    return m_translation;
}

auto Node::rotation() const noexcept -> const glm::quat&
{
    return m_rotation;
}

auto Node::rotation() noexcept -> glm::quat&
{
    return m_rotation;
}

auto Node::scale() const noexcept -> const glm::vec3&
{
    return m_scale;
}

auto Node::scale() noexcept -> glm::vec3&
{
    return m_scale;
}

auto Node::local_matrix() const -> glm::mat4
{
    return glm::translate(glm::mat4(1.f), m_translation) * glm::mat4_cast(m_rotation)
         * glm::scale(glm::mat4(1.f), m_scale);
}

auto Node::matrix(const Model& model) const -> glm::mat4
{
    glm::mat4 result{ local_matrix() };
    for (std::optional<size_t> parent_index{ m_parent_index }; parent_index.has_value();
         parent_index = model.nodes().at(parent_index.value()).m_parent_index)
    {
        result = model.nodes().at(parent_index.value()).local_matrix() * result;
    }
    return result;
}

auto Node::mesh_index() const noexcept -> std::optional<size_t>
{
    return m_mesh_index;
}

auto Node::mesh(const Model& model
) const -> std::optional<std::reference_wrapper<const Mesh>>
{
    return m_mesh_index.transform([&](const size_t index) {
        return model.meshes().at(index);
    });
}

}   // namespace core::gltf
