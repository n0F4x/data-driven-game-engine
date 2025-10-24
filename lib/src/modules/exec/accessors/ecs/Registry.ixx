export module ddge.modules.exec.accessors.ecs:Registry;

import :locks.ExclusiveRegistryLock;

import ddge.modules.ecs;
import ddge.modules.exec.locks.Lockable;

import ddge.utility.containers.Ref;

namespace ddge::exec::accessors {

inline namespace ecs {

export class Registry : public util::Ref<ddge::ecs::Registry>,
                        public Lockable<ExclusiveRegistryLock> {
    using Base = util::Ref<ddge::ecs::Registry>;

public:
    using Base::Base;
};

}   // namespace ecs

}   // namespace ddge::exec::accessors
