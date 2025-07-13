module;

#include <memory>
#include <type_traits>

export module core.assets.Handle;

import core.assets.asset_c;

import utility.containers.Any;
import utility.contracts;

namespace core::assets {

export template <typename Asset_T>
    requires asset_c<std::remove_const_t<Asset_T>>
using Handle = std::shared_ptr<Asset_T>;

}   // namespace core::assets
