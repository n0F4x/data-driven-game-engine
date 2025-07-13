module;

#include <memory>
#include <type_traits>

export module core.assets.WeakHandle;

import core.assets.asset_c;

import utility.containers.Any;
import utility.contracts;

namespace core::assets {

export template <typename Asset_T>
    requires asset_c<std::remove_const_t<Asset_T>>
using WeakHandle = std::weak_ptr<Asset_T>;

}   // namespace core::assets
