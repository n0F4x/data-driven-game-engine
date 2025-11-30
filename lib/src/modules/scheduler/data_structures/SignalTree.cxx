module;

#include <bit>
#include <cassert>
#include <cstdint>
#include <optional>

#include <tl/function_ref.hpp>

#include "utility/contract_macros.hpp"

module ddge.modules.scheduler.data_structures.SignalTree;

import ddge.utility.contracts;

[[nodiscard]]
constexpr auto parent_index_of(const uint32_t node_index) noexcept -> uint32_t
{
    return (node_index + 1) / 2 - 1;
}

[[nodiscard]]
constexpr auto left_child_index_of(const uint32_t node_index) noexcept -> uint32_t
{
    return (node_index + 1) * 2 - 1;
}

[[nodiscard]]
constexpr auto right_child_index_of(const uint32_t node_index) noexcept -> uint32_t
{
    return (node_index + 1) * 2;
}

ddge::scheduler::SignalTree::SignalTree(const uint32_t number_of_levels)
    : m_precondition{ number_of_levels },
      m_nodes((0x1u << number_of_levels) - 1)
{}

auto ddge::scheduler::SignalTree::try_set_one(const LeafIndex leaf_index) -> bool
{
    const NodeIndex node_index{ leaf_index + number_of_branches() };
    PRECOND(node_index < m_nodes.size());

    if (m_nodes[node_index].counter.exchange(1) == 1) {
        return false;
    }

    set_branch(::parent_index_of(node_index));

    return true;
}

auto ddge::scheduler::SignalTree::try_unset_one(const TravelsalTactic tactic)
    -> std::optional<LeafIndex>
{
    return try_unset_one(0, tactic);
}

auto ddge::scheduler::SignalTree::try_unset_one_at(const LeafIndex leaf_index) -> bool
{
    return try_unset_one_at(0, leaf_index + number_of_branches());
}

auto ddge::scheduler::SignalTree::full() const noexcept -> bool
{
    return m_nodes[0].counter.load() == number_of_leaves();
}

auto ddge::scheduler::SignalTree::number_of_levels() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(std::bit_width(m_nodes.size()));
}

auto ddge::scheduler::SignalTree::number_of_leaves() const noexcept -> uint32_t
{
    return 0x1u << (number_of_levels() - 1);
}

auto ddge::scheduler::SignalTree::number_of_branches() const noexcept -> uint32_t
{
    return number_of_nodes() - number_of_leaves();
}

auto ddge::scheduler::SignalTree::number_of_nodes() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_nodes.size());
}

ddge::scheduler::SignalTree::Precondition::Precondition(const uint32_t number_of_levels)
{
    PRECOND(number_of_levels >= minimum_number_of_levels());
}

auto ddge::scheduler::SignalTree::is_leaf_index(const NodeIndex node_index) const noexcept
    -> bool
{
    return node_index >= number_of_branches();
}

auto ddge::scheduler::SignalTree::set_branch(const NodeIndex node_index) -> void
{
    PRECOND(node_index < (number_of_branches()));

    m_nodes[node_index].counter.fetch_add(1);

    if (node_index == 0) {
        return;
    }

    set_branch(::parent_index_of(node_index));
}

auto ddge::scheduler::SignalTree::try_unset_one(
    const NodeIndex                                 node_index,
    const tl::function_ref<TravelsalBias(uint32_t)> strategy
) -> std::optional<LeafIndex>
{
    Counter::value_type counter{ m_nodes[node_index].counter.load() };
    do {
        if (counter == 0) {
            return std::nullopt;
        }
    } while (!m_nodes[node_index].counter.compare_exchange_weak(counter, counter - 1));

    const TravelsalBias travelsal_bias{ strategy(node_index) };

    const NodeIndex left_child_index{ ::left_child_index_of(node_index) };
    const NodeIndex right_child_index{ ::right_child_index_of(node_index) };

    const bool               is_last_branch{ is_leaf_index(left_child_index) };
    NodeIndex                visited_child_index{ travelsal_bias == TravelsalBias::eLeft
                                                      ? left_child_index
                                                      : right_child_index };
    std::optional<LeafIndex> result;
    while (!result.has_value()) {
        result              = is_last_branch ? try_unset_leaf(visited_child_index)
                                             : try_unset_one(visited_child_index, strategy);
        visited_child_index = visited_child_index == left_child_index ? right_child_index
                                                                      : left_child_index;
    }
    return result;
}

auto ddge::scheduler::SignalTree::try_unset_one_at(
    const NodeIndex node_index,
    const NodeIndex target_index
) -> bool
{
    Counter::value_type counter{ m_nodes[node_index].counter.load() };
    do {
        if (counter == 0) {
            return false;
        }
    } while (!m_nodes[node_index].counter.compare_exchange_weak(counter, counter - 1));

    const uint32_t  level{ static_cast<uint32_t>(std::bit_width(node_index + 1)) - 1u };
    const NodeIndex child_index{
        ::left_child_index_of(node_index)
        + (((target_index + 1) >> (number_of_levels() - level - 2)) & 1u)
    };
    assert(!is_leaf_index(child_index) || child_index == target_index);

    const bool result = is_leaf_index(child_index)
                          ? try_unset_leaf(child_index).has_value()
                          : try_unset_one_at(child_index, target_index);

    if (!result) {
        m_nodes[node_index].counter.fetch_add(1);
    }

    return result;
}

auto ddge::scheduler::SignalTree::try_unset_leaf(const NodeIndex node_index)
    -> std::optional<LeafIndex>
{
    PRECOND(is_leaf_index(node_index));

    if (Counter::value_type expected{ 1 };
        m_nodes[node_index].counter.compare_exchange_strong(expected, 0) == false)
    {
        return std::nullopt;
    }

    return node_index - number_of_branches();
}
