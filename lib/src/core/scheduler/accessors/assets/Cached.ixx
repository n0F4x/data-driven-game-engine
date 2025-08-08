export module core.scheduler.accessors.assets.Cached;

import core.assets;

import utility.containers.Ref;

namespace core::scheduler::accessors {

inline namespace assets {

export template <core::assets::loader_c Loader_T>
class Cached : public util::Ref<core::assets::Cached<Loader_T>> {
    using Base = util::Ref<core::assets::Cached<Loader_T>>;

public:
    using Loader = Loader_T;

    using Base::Base;
};

}   // namespace assets

}   // namespace core::scheduler::accessors
