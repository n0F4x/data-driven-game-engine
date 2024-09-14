#pragma once

#include <memory>

namespace core::cache {

template <typename Resource>
using Handle = std::shared_ptr<Resource>;

template <typename Resource>
[[nodiscard]]
auto make_handle(auto&&... args) -> Handle<Resource>;

}   // namespace core::cache

#include "Handle.inl"
