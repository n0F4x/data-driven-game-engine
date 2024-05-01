#pragma once

#include <gsl/pointers>

namespace core::cache {

template <typename Resource>
using Handle = gsl::not_null<std::shared_ptr<Resource>>;

template <typename Resource>
[[nodiscard]]
auto make_handle(auto&&... t_args) -> Handle<Resource>;

}   // namespace core::cache

#include "Handle.inl"
