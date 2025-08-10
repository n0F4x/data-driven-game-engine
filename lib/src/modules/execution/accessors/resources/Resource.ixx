module;

#include <type_traits>

export module ddge.modules.execution.accessors.resources.Resource;

import ddge.modules.resources.resource_c;

import ddge.utility.containers.Ref;

namespace ddge::exec::accessors {

inline namespace resources {

export template <typename Resource_T>
    requires ddge::resources::resource_c<std::remove_const_t<Resource_T>>
class Resource : public util::Ref<Resource_T> {
    using Base = util::Ref<Resource_T>;

public:
    using Underlying = Resource_T;

    using Base::Base;
};

}   // namespace resources

}   // namespace ddge::exec::accessors
