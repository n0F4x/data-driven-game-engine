module;

#include <type_traits>

export module core.scheduler.accessors.resources.Resource;

import core.resources.resource_c;

import utility.containers.Ref;

namespace core::scheduler::accessors {

inline namespace resources {

export template <typename Resource_T>
    requires core::resources::resource_c<std::remove_const_t<Resource_T>>
class Resource : public util::Ref<Resource_T> {
    using Base = util::Ref<Resource_T>;

public:
    using Underlying = Resource_T;

    using Base::Base;
};

}   // namespace resources

}   // namespace core::scheduler::accessors
