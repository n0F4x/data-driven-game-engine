#pragma once

#include "Buffer.hpp"

namespace core::renderer::base {

template <typename T = std::byte>
class SeqWriteBuffer : public Buffer {
public:
    using Buffer::Buffer;

    template <size_t E>
    auto set(std::span<const T, E> data, size_t offset = 0) const -> void;
    auto set(const T& data, size_t offset = 0) const -> void;

    [[nodiscard]]
    auto size() const noexcept -> size_t;
};

}   // namespace core::renderer

#include "SeqWriteBuffer.inl"
