export module ddge.modules.scheduler.accessors.assets.Cached;

import ddge.modules.assets;

import ddge.utility.containers.Ref;

namespace ddge::scheduler::accessors {

inline namespace assets {

export template <ddge::assets::loader_c Loader_T>
class Cached : public util::Ref<ddge::assets::Cached<Loader_T>> {
    using Base = util::Ref<ddge::assets::Cached<Loader_T>>;

public:
    using Loader = Loader_T;

    using Base::Base;
};

}   // namespace assets

}   // namespace ddge::scheduler::accessors
