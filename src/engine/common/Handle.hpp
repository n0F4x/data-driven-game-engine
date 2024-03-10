#pragma once

#include <memory>

#include <gsl/pointers>

namespace engine {

template <typename Resource>
using Handle = gsl::not_null<std::shared_ptr<Resource>>;

template <typename Resource>
[[nodiscard]] auto make_handle(auto&&... t_args) -> Handle<Resource>;

}   // namespace engine

#include "Handle.inl"
