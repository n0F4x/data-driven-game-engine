module;

#include <algorithm>

export module ddge.modules.vulkan.extension_inspection.extension_is_promoted_to_vulkan1x;

import vulkan_hpp;

import ddge.modules.vulkan.extension_inspection.extensions_promoted_to_vulkan1x;
import ddge.util.containers.StringLiteral;

namespace ddge::vulkan {

export [[nodiscard]]
auto extension_is_promoted_to_vulkan11(util::StringLiteral extension_name) -> bool;

export [[nodiscard]]
auto extension_is_promoted_to_vulkan12(util::StringLiteral extension_name) -> bool;

export [[nodiscard]]
auto extension_is_promoted_to_vulkan13(util::StringLiteral extension_name) -> bool;

export [[nodiscard]]
auto extension_is_promoted_to_vulkan14(util::StringLiteral extension_name) -> bool;

export [[nodiscard]]
auto extension_is_promoted_to_vulkan1x(util::StringLiteral extension_name) -> bool;

}   // namespace ddge::vulkan

module :private;

namespace ddge::vulkan {

auto extension_is_promoted_to_vulkan11(const util::StringLiteral extension_name) -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan11(), extension_name);
}

auto extension_is_promoted_to_vulkan12(const util::StringLiteral extension_name) -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan12(), extension_name);
}

auto extension_is_promoted_to_vulkan13(const util::StringLiteral extension_name) -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan13(), extension_name);
}

auto extension_is_promoted_to_vulkan14(const util::StringLiteral extension_name) -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan14(), extension_name);
}

auto extension_is_promoted_to_vulkan1x(const util::StringLiteral extension_name) -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan1x(), extension_name);
}

}   // namespace ddge::vulkan
