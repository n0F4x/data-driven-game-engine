#pragma once

#include "SeqWriteBuffer.hpp"

namespace core::renderer::base {

template <typename T>
class RandomAccessBuffer : public SeqWriteBuffer<T> {
public:
    using SeqWriteBuffer<T>::SeqWriteBuffer;
};

}   // namespace core::renderer
