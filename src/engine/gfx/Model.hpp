#pragma once

#include <tl/optional.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace engine::gfx {

class ModelFactory;

class Model {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
    };

    struct Primitive {
        uint32_t first_index_index{};
        uint32_t index_count{};
        uint32_t vertex_count{};
    };

    struct Mesh {
        std::vector<Primitive> primitives;

        struct UniformBlock {
            glm::mat4 matrix{ glm::identity<glm::mat4>() };
        } uniform_block;
    };

    struct Node {
        Node*              parent{};
        std::vector<Node>  children;
        tl::optional<Mesh> mesh;
        glm::mat4          matrix{ glm::identity<glm::mat4>() };
    };

    [[nodiscard]] auto vertices() const noexcept -> const std::vector<Vertex>&;
    [[nodiscard]] auto indices() const noexcept -> const std::vector<uint32_t>&;
    [[nodiscard]] auto nodes() const noexcept -> const std::vector<Node>&;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend ModelFactory;

    ///*************///
    ///  Variables  ///
    ///*************///
    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Node>     m_nodes;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Model(
        std::vector<Vertex>&&   t_vertices,
        std::vector<uint32_t>&& t_indices,
        std::vector<Node>&&     t_nodes
    ) noexcept;
};

}   // namespace engine::gfx
