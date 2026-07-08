module;

#include <magic_enum/magic_enum.hpp>

export module ddge.scheduler.locks.CriticalSectionType;

import ddge.scheduler.locks.CriticalSection;

import ddge.util.meta.type_traits.type_list.type_list_size;

namespace ddge::scheduler {

export enum struct CriticalSectionType {
    eExclusive,
    eShared,
};

static_assert(
    util::meta::type_list_size_v<CriticalSection>
    == magic_enum::enum_count<CriticalSectionType>()
);

}   // namespace ddge::scheduler
