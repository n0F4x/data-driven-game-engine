module;

#include <type_traits>

export module ddge.modules.exec.accessors.resources:Resource;

import ddge.modules.resources.resource_c;
import ddge.modules.exec.locks.Lockable;
import ddge.modules.exec.locks.ReaderLock;
import ddge.modules.exec.locks.WriterLock;

import ddge.utility.containers.Ref;
import ddge.utility.meta.type_traits.const_like;

namespace ddge::exec::accessors {

inline namespace resources {

template <typename Resource_T>
struct ResourceLock : std::conditional_t<
                          std::is_const_v<Resource_T>,
                          ReaderLock<ResourceLock<std::remove_const_t<Resource_T>>>,
                          WriterLock<ResourceLock<std::remove_const_t<Resource_T>>>> {};

export template <typename Resource_T>
    requires ddge::resources::resource_c<std::remove_const_t<Resource_T>>
class Resource : public util::Ref<Resource_T>, public Lockable<ResourceLock<Resource_T>> {
    using Base = util::Ref<Resource_T>;

public:
    using Underlying = Resource_T;

    using Base::Base;
};

}   // namespace resources

}   // namespace ddge::exec::accessors
