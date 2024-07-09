#pragma once

#include <gsl-lite/gsl-lite.hpp>

#include "core/utility/tuple-like.hpp"

namespace core::cache {

template <typename Resource>
using Handle = gsl_lite::not_null_ic<std::shared_ptr<Resource>>;

template <typename Resource>
[[nodiscard]]
auto make_handle(auto&&... args) -> Handle<Resource>;

template <typename Resource, tuple_like Tuple>
[[nodiscard]]
auto make_handle(Tuple&& tuple) -> Handle<Resource>;

}   // namespace core::cache

#include "Handle.inl"
