#pragma once

#include "Buffer.hpp"

namespace core::renderer {

class MappedBuffer : public Buffer {
public:
    using Buffer::Buffer;

    template <typename T>
    auto set(const T& data) const -> void;
};

}   // namespace core::renderer

#include "MappedBuffer.inl"
