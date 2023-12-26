#pragma once

#include <vk_mem_alloc.h>

namespace engine::vma {

class Allocator {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(VmaAllocator t_allocator) noexcept;
    Allocator(Allocator&&) noexcept;
    ~Allocator() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Allocator&&) noexcept -> Allocator& = default;
    [[nodiscard]] auto operator*() const noexcept -> VmaAllocator;
    [[nodiscard]] auto operator->() const noexcept -> const VmaAllocator*;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    VmaAllocator m_allocator;
};

}   // namespace engine::vma
