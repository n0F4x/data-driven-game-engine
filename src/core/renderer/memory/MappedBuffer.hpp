#pragma once

#include "Buffer.hpp"

namespace core::renderer {

class MappedBuffer : public Buffer {
public:
    using Buffer::Buffer;

    template<typename T>
    auto set(const T& t_data) const -> void;
};

}   // namespace core::renderer

#include "MappedBuffer.inl"
