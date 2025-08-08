export module modules.scheduler.accessors.assets.Cached;

import modules.assets;

import utility.containers.Ref;

namespace modules::scheduler::accessors {

inline namespace assets {

export template <modules::assets::loader_c Loader_T>
class Cached : public util::Ref<modules::assets::Cached<Loader_T>> {
    using Base = util::Ref<modules::assets::Cached<Loader_T>>;

public:
    using Loader = Loader_T;

    using Base::Base;
};

}   // namespace assets

}   // namespace modules::scheduler::accessors
