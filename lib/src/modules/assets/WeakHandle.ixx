module;

#include <memory>
#include <type_traits>

export module modules.assets.WeakHandle;

import modules.assets.asset_c;

import utility.containers.Any;
import utility.contracts;

namespace modules::assets {

export template <typename Asset_T>
    requires asset_c<std::remove_const_t<Asset_T>>
using WeakHandle = std::weak_ptr<Asset_T>;

}   // namespace modules::assets
