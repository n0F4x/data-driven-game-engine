export module extensions.scheduler.accessors.assets.Cached;

import core.assets;

import utility.containers.Ref;

namespace extensions::scheduler::accessors {

inline namespace assets {

export template <core::assets::loader_c Loader_T>
class Cached : public util::Ref<core::assets::Cached<Loader_T>> {
    using Base = util::Ref<core::assets::Cached<Loader_T>>;

public:
    using Base::Base;
};

}   // namespace assets

}   // namespace extensions::scheduler::accessors
