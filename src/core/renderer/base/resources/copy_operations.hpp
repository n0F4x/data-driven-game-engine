#pragma once

#include "Allocation.hpp"

namespace core::renderer::base {

// NOLINTNEXTLINE(*-member-init)
struct CopyRegion {
    std::reference_wrapper<const Allocation> allocation;
    vk::DeviceSize                     offset{};
};

auto copy(const void* source, const CopyRegion& destination, vk::DeviceSize size) -> void;

}   // namespace core::renderer::base
