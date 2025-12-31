module;

#include <ranges>
#include <span>
#include <vector>

export module ddge.modules.vulkan.extension_inspection.extensions_promoted_to_vulkan1x;

import vulkan_hpp;

import ddge.utility.containers.StringLiteral;

namespace ddge::vulkan {

export [[nodiscard]]
auto extensions_promoted_to_vulkan11() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
auto extensions_promoted_to_vulkan12() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
auto extensions_promoted_to_vulkan13() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
auto extensions_promoted_to_vulkan14() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
auto extensions_promoted_to_vulkan1x() -> std::span<const util::StringLiteral>;

}   // namespace ddge::vulkan

module :private;

namespace ddge::vulkan {

auto query_extensions_promoted_to(const util::StringLiteral promoted_to)
    -> std::vector<util::StringLiteral>
{
    std::vector<util::StringLiteral> result;
    for (const auto& [extension, x_promoted_to] : vk::getPromotedExtensions()) {
        if (x_promoted_to == promoted_to.get()) {
            result.push_back(util::StringLiteral::unsafe_create(extension.c_str()));
        }
    }

    return result;
}

auto extensions_promoted_to_vulkan11() -> std::span<const util::StringLiteral>
{
    static const std::vector<util::StringLiteral> result{
        query_extensions_promoted_to("VK_VERSION_1_1")
    };

    return result;
}

auto extensions_promoted_to_vulkan12() -> std::span<const util::StringLiteral>
{
    static const std::vector<util::StringLiteral> result{
        query_extensions_promoted_to("VK_VERSION_1_2")
    };

    return result;
}

auto extensions_promoted_to_vulkan13() -> std::span<const util::StringLiteral>
{
    static const std::vector<util::StringLiteral> result{
        query_extensions_promoted_to("VK_VERSION_1_3")
    };

    return result;
}

auto extensions_promoted_to_vulkan14() -> std::span<const util::StringLiteral>
{
    static const std::vector<util::StringLiteral> result{
        query_extensions_promoted_to("VK_VERSION_1_4")
    };

    return result;
}

auto extensions_promoted_to_vulkan1x() -> std::span<const util::StringLiteral>
{
    static const std::vector<util::StringLiteral> result{ [] {
        std::vector<util::StringLiteral>          promoted_extensions;

        promoted_extensions.reserve(extensions_promoted_to_vulkan11().size());
        promoted_extensions.reserve(extensions_promoted_to_vulkan12().size());
        promoted_extensions.reserve(extensions_promoted_to_vulkan13().size());
        promoted_extensions.reserve(extensions_promoted_to_vulkan14().size());

        promoted_extensions.append_range(extensions_promoted_to_vulkan11());
        promoted_extensions.append_range(extensions_promoted_to_vulkan12());
        promoted_extensions.append_range(extensions_promoted_to_vulkan13());
        promoted_extensions.append_range(extensions_promoted_to_vulkan14());

        return promoted_extensions;
    }() };

    return result;
}

}   // namespace ddge::vulkan
