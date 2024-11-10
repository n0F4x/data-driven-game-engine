#pragma once

namespace core::renderer {

template <typename Self>
auto VertexLayout::add_attribute(this Self&& self, VertexAttribute attribute) -> Self
{
    self.m_attributes.push_back(attribute);
    return std::forward<Self>(self);
}

}   // namespace core::renderer
