#pragma once

#include <vulkan/vulkan.hpp>

#include "engine/utility/vma/Buffer.hpp"

namespace engine::renderer {

class MeshBuffer {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Vertices {
        vulkan::vma::Buffer buffer;
    };

    struct Indices {
        uint32_t            count{};
        vulkan::vma::Buffer buffer;
    };

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit MeshBuffer(Vertices&& t_vertices, Indices&& t_indices) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto bind(vk::CommandBuffer t_command_buffer) const noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Vertices m_vertices;
    Indices  m_indices;
};

}   // namespace engine::renderer
