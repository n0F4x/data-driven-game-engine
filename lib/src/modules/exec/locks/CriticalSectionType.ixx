module;

#include <magic_enum/magic_enum.hpp>

export module ddge.modules.exec.locks.CriticalSectionType;

import ddge.modules.exec.locks.CriticalSection;

import ddge.utility.meta.type_traits.type_list.type_list_size;

namespace ddge::exec {

export enum struct CriticalSectionType {
    eExclusive,
    eShared,
};

static_assert(
    util::meta::type_list_size_v<CriticalSection>
    == magic_enum::enum_count<CriticalSectionType>()
);

}   // namespace ddge::exec
