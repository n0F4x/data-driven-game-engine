module;

#include <cstdint>

export module core.ecs.Entity;

import utility.Strong;

namespace core::ecs {

export using ID    = ::util::Strong<uint64_t>;
export using Key   = ::util::Strong<uint64_t>;
export using Index = ::util::Strong<uint32_t>;

export [[nodiscard]]
auto make_id(ID::Underlying value) noexcept -> ID;
export [[nodiscard]]
auto make_key(Key::Underlying value) noexcept -> Key;
export [[nodiscard]]
auto make_index(Index::Underlying value) noexcept -> Index;

}   // namespace core::ecs

auto core::ecs::make_id(const core::ecs::ID::Underlying value) noexcept -> ID
{
    return ID{ value };
}

auto core::ecs::make_key(const Key::Underlying value) noexcept -> Key
{
    return Key{ value };
}

auto core::ecs::make_index(const Index::Underlying value) noexcept -> Index
{
    return Index{ value };
}
