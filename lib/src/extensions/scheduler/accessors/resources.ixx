module;

#include <type_traits>

export module extensions.scheduler.accessors.resources;

import core.resources.resource_c;

import utility.containers.Ref;

namespace extensions::scheduler::accessors::resources {

export template <typename Resource_T>
    requires core::resource::resource_c<std::remove_const_t<Resource_T>>
class Res : public util::Ref<Resource_T> {
    using Base = util::Ref<Resource_T>;

public:
    using Base::Base;
};

}   // namespace extensions::scheduler::accessors::resource
