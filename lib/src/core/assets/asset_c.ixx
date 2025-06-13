export module core.assets.asset_c;

import utility.meta.concepts.nothrow_movable;

namespace core::assets {

export template <typename T>
concept asset_c = util::meta::nothrow_movable_c<T>;

}   // namespace core::assets
