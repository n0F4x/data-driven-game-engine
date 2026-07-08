module;

#include <cstdint>
#include <memory_resource>
#include <string_view>

export module ddge.app.v2.registry.ReverseDependencyChainNode;

namespace ddge::app::v2 {

export struct ReverseDependencyChainNode {
    const ReverseDependencyChainNode* previous{};
    uint64_t                   hash;
    std::string_view           name;

    [[nodiscard]]
    auto contains(uint64_t hash) const noexcept -> bool;

    [[nodiscard]]
    auto format(const std::pmr::string::allocator_type& allocator) const
        -> std::pmr::string;

private:
    auto format(std::pmr::string& out, std::size_t capacity) const -> void;
};

}   // namespace ddge::app::v2
