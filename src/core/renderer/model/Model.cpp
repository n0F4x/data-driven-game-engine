#include "Model.hpp"

#include <glm/gtc/quaternion.hpp>

namespace core::renderer {

auto Model::Node::local_matrix() const -> glm::mat4
{
    return glm::translate(glm::mat4(1.f), translation) * glm::mat4_cast(rotation)
         * glm::scale(glm::mat4(1.f), scale);
}

auto Model::Node::matrix() const -> glm::mat4
{
    glm::mat4 result{ local_matrix() };
    for (Node* p{ parent }; p != nullptr; p = p->parent) {
        result = p->local_matrix() * result;
    }
    return result;
}

auto Model::hash(const std::filesystem::path& t_filepath) noexcept -> size_t
{
    return std::filesystem::hash_value(t_filepath);
}

[[nodiscard]] auto hash_value(const Model& t_model) noexcept -> size_t
{
    return Model::hash(t_model.m_filepath);
}

}   // namespace core::renderer

namespace std {

auto hash<core::renderer::Model>::operator()(const core::renderer::Model& t_model) const
    -> size_t
{
    return core::renderer::hash_value(t_model);
}

}   // namespace std
