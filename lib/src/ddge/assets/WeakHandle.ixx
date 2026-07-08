module;

#include <memory>
#include <type_traits>

export module ddge.assets.WeakHandle;

import ddge.assets.asset_c;

import ddge.util.containers.Any;
import ddge.util.contracts;

namespace ddge::assets {

export template <typename Asset_T>
    requires asset_c<std::remove_const_t<Asset_T>>
using WeakHandle = std::weak_ptr<Asset_T>;

}   // namespace ddge::assets
