module;

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <optional>
#include <ranges>
#include <vector>

#include <tl/function_ref.hpp>

#include "utility/contracts_macros.hpp"

export module ddge.modules.execution.scheduler.SignalTree;

import ddge.utility.contracts;

namespace ddge::exec {

struct NodeData {
    std::atomic_int32_t count{};
};

using NodeIndex = uint32_t;

struct RootNode {
    NodeIndex left_index{ 1 };
    NodeIndex right_index{ 2 };
    NodeData  data;
};

struct BranchNode {
    NodeIndex parent_index{};
    NodeIndex left_index{};
    NodeIndex right_index{};
    NodeData  data;
};

struct LeafNode {
    NodeIndex parent_index{};
    NodeData  data;
};

export using SignalIndex = NodeIndex;

export enum struct TravelsalBias : std::int8_t {
    eLeft,
    eRight,
};

/**
 * Multi-producer multi-consumer
 */
export class SignalTree {
public:
    explicit SignalTree(uint32_t number_of_levels);

    auto set(SignalIndex index) -> void;
    [[nodiscard]]
    auto unset_one(tl::function_ref<TravelsalBias(uint32_t level_index)> strategy)
        -> std::optional<SignalIndex>;

    [[nodiscard]]
    auto size() const -> uint32_t;
    [[nodiscard]]
    auto number_of_levels() const -> uint32_t;

private:
    uint32_t                m_number_of_levels;
    RootNode                m_root_node;
    std::vector<BranchNode> m_branch_nodes;
    std::vector<LeafNode>   m_leaf_nodes;

    auto connect_nodes() -> void;

    [[nodiscard]]
    auto first_leaf_index() const noexcept -> NodeIndex;
    [[nodiscard]]
    auto is_leaf_index(NodeIndex node_index) const noexcept -> bool;

    auto set_branch(NodeIndex node_index) -> void;
    auto set_root() -> void;

    auto unset_one_from_branch(
        uint32_t                                  level_index,
        NodeIndex                                 node_index,
        tl::function_ref<TravelsalBias(uint32_t)> strategy
    ) -> std::optional<SignalIndex>;
    auto unset_one_from_leaf(NodeIndex node_index) -> std::optional<SignalIndex>;
};

}   // namespace ddge::exec

module :private;

[[nodiscard]]
auto number_of_nodes(const uint32_t number_of_levels) noexcept -> uint32_t
{
    return (0x1u << number_of_levels) - 1;
}

[[nodiscard]]
auto number_of_leaves(const uint32_t number_of_levels) noexcept -> uint32_t
{
    return 0x1u << (number_of_levels - 1);
}

[[nodiscard]]
auto number_of_branches(const uint32_t number_of_levels) noexcept -> uint32_t
{
    return ::number_of_nodes(number_of_levels) - ::number_of_leaves(number_of_levels) - 1;
}

ddge::exec::SignalTree::SignalTree(const uint32_t number_of_levels)
    : m_number_of_levels{ number_of_levels },
      m_branch_nodes(::number_of_branches(number_of_levels)),
      m_leaf_nodes(::number_of_leaves(number_of_levels))
{
    PRECOND(number_of_levels >= 3);

    connect_nodes();
}

auto ddge::exec::SignalTree::set(const SignalIndex index) -> void
{
    assert(index < m_leaf_nodes.size());

    LeafNode& leaf_node{ m_leaf_nodes[index] };

    if (leaf_node.data.count.exchange(1) == 1) {
        return;
    }

    set_branch(leaf_node.parent_index);
}

auto ddge::exec::SignalTree::unset_one(
    const tl::function_ref<TravelsalBias(uint32_t)> strategy
) -> std::optional<SignalIndex>
{
    if (--m_root_node.data.count < 0) {
        ++m_root_node.data.count;
        return std::nullopt;
    }

    const TravelsalBias travelsal_bias{ strategy(0) };

    const NodeIndex preferred_child_index{ travelsal_bias == TravelsalBias::eLeft
                                               ? m_root_node.left_index
                                               : m_root_node.right_index };
    const NodeIndex less_preferred_child_index{ preferred_child_index
                                                        == m_root_node.left_index
                                                    ? m_root_node.right_index
                                                    : m_root_node.left_index };

    std::optional<SignalIndex> result;
    NodeIndex                  visited_child_index{ preferred_child_index };
    while (!result.has_value()) {
        result              = unset_one_from_branch(1, visited_child_index, strategy);
        visited_child_index = visited_child_index == preferred_child_index
                                ? less_preferred_child_index
                                : preferred_child_index;
    }
    return result;
}

auto ddge::exec::SignalTree::size() const -> uint32_t
{
    return static_cast<uint32_t>(m_leaf_nodes.size());
}

auto ddge::exec::SignalTree::number_of_levels() const -> uint32_t
{
    return m_number_of_levels;
}

auto ddge::exec::SignalTree::connect_nodes() -> void
{
    for (const uint32_t level :
         std::views::iota(1u)
             | std::views::take(std::max(static_cast<int>(m_number_of_levels) - 3, 0)))
    {
        const uint32_t  number_of_nodes_on_level{ 0x1u << level };
        const NodeIndex first_node_index{ (0x1u << level) - 1 };
        const NodeIndex next_level_first_index{ (0x1u << (level + 1)) - 1 };

        for (const NodeIndex node_index : std::views::iota(
                 first_node_index, first_node_index + number_of_nodes_on_level
             ))
        {
            const NodeIndex left_index{ next_level_first_index
                                        + (node_index - first_node_index) * 2 };
            const NodeIndex right_index{ next_level_first_index
                                         + (node_index - first_node_index) * 2 + 1 };

            m_branch_nodes[node_index - 1].left_index   = left_index;
            m_branch_nodes[left_index - 1].parent_index = node_index;

            m_branch_nodes[node_index - 1].right_index   = right_index;
            m_branch_nodes[right_index - 1].parent_index = node_index;
        }
    }

    {
        const uint32_t  almost_last_level{ m_number_of_levels - 2 };
        const NodeIndex number_of_nodes_on_almost_last_level{ 0x1u << almost_last_level };
        const NodeIndex first_node_index{ (0x1u << almost_last_level) - 1 };
        const NodeIndex last_level_first_index{ (0x1u << (almost_last_level + 1)) - 1 };

        for (const NodeIndex node_index : std::views::iota(
                 first_node_index, first_node_index + number_of_nodes_on_almost_last_level
             ))
        {
            const NodeIndex left_index{ last_level_first_index
                                        + (node_index - first_node_index) * 2 };
            const NodeIndex right_index{ last_level_first_index
                                         + (node_index - first_node_index) * 2 + 1 };

            m_branch_nodes[node_index - 1].left_index                      = left_index;
            m_leaf_nodes[left_index - last_level_first_index].parent_index = node_index;

            m_branch_nodes[node_index - 1].right_index                      = right_index;
            m_leaf_nodes[right_index - last_level_first_index].parent_index = node_index;
        }
    }
}

auto ddge::exec::SignalTree::first_leaf_index() const noexcept -> NodeIndex
{
    return (0x1u << (m_number_of_levels - 1)) - 1;
}

auto ddge::exec::SignalTree::is_leaf_index(const NodeIndex node_index) const noexcept
    -> bool
{
    return node_index >= first_leaf_index();
}

auto ddge::exec::SignalTree::set_branch(const NodeIndex node_index) -> void
{
    if (node_index == 0) {
        set_root();
        return;
    }

    BranchNode& branch_node{ m_branch_nodes[node_index - 1] };

    ++branch_node.data.count;

    set_branch(branch_node.parent_index);
}

auto ddge::exec::SignalTree::set_root() -> void
{
    ++m_root_node.data.count;
}

auto ddge::exec::SignalTree::unset_one_from_branch(
    const uint32_t                                  level_index,
    const NodeIndex                                 node_index,
    const tl::function_ref<TravelsalBias(uint32_t)> strategy
) -> std::optional<SignalIndex>
{
    BranchNode& branch_node{ m_branch_nodes[node_index - 1] };

    if (--branch_node.data.count < 0) {
        ++branch_node.data.count;
        return std::nullopt;
    }

    const TravelsalBias travelsal_bias{ strategy(level_index) };

    const NodeIndex preferred_child_index{ travelsal_bias == TravelsalBias::eLeft
                                               ? branch_node.left_index
                                               : branch_node.right_index };
    const NodeIndex less_preferred_child_index{ preferred_child_index
                                                        == branch_node.left_index
                                                    ? branch_node.right_index
                                                    : branch_node.left_index };

    std::optional<SignalIndex> result;
    NodeIndex                  visited_child_index{ preferred_child_index };
    const bool                 is_last_branch{ is_leaf_index(branch_node.left_index) };
    while (!result.has_value()) {
        result =
            is_last_branch
                ? unset_one_from_leaf(visited_child_index)
                : unset_one_from_branch(level_index + 1, visited_child_index, strategy);
        visited_child_index = visited_child_index == preferred_child_index
                                ? less_preferred_child_index
                                : preferred_child_index;
    }
    return result;
}

auto ddge::exec::SignalTree::unset_one_from_leaf(const NodeIndex node_index)
    -> std::optional<SignalIndex>
{
    const NodeIndex leaf_index{ node_index - first_leaf_index() };

    if (LeafNode & leaf_node{ m_leaf_nodes[leaf_index] }; --leaf_node.data.count < 0) {
        ++leaf_node.data.count;
        return std::nullopt;
    }

    return leaf_index;
}
