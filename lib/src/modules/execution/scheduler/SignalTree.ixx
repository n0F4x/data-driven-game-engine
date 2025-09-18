module;

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include <tl/function_ref.hpp>

export module ddge.modules.execution.scheduler.SignalTree;

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
    constexpr static std::integral_constant<uint32_t, 3> minimum_number_of_levels;
    constexpr static std::
        integral_constant<uint32_t, 0x1u << (minimum_number_of_levels - 1)>
            minimum_capacity;

    explicit SignalTree(uint32_t number_of_levels);

    auto set(SignalIndex index) -> bool;

    [[nodiscard]]
    auto try_unset_one(tl::function_ref<TravelsalBias(uint32_t level_index)> strategy)
        -> std::optional<SignalIndex>;

    [[nodiscard]]
    auto capacity() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_levels() const noexcept -> uint32_t;

private:
    uint32_t                  m_number_of_levels;
    // TODO: use std::indirect
    std::unique_ptr<RootNode> m_root_node{ std::make_unique<RootNode>() };
    std::vector<BranchNode>   m_branch_nodes;
    std::vector<LeafNode>     m_leaf_nodes;

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
