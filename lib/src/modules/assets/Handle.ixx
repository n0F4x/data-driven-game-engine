module;

#include <memory>
#include <type_traits>

export module ddge.modules.assets.Handle;

import ddge.modules.assets.asset_c;

import ddge.utility.containers.Any;
import ddge.utility.contracts;

namespace ddge::assets {

export template <typename Asset_T>
    requires asset_c<std::remove_const_t<Asset_T>>
using Handle = std::shared_ptr<Asset_T>;

}   // namespace ddge::assets
