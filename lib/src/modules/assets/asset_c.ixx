export module ddge.modules.assets.asset_c;

import ddge.utility.meta.concepts.nothrow_movable;

namespace ddge::assets {

export template <typename T>
concept asset_c = util::meta::nothrow_movable_c<T>;

}   // namespace ddge::assets
