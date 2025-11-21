module;

#include <atomic>
#include <cstdint>
#include <optional>
#include <vector>

#include <tl/function_ref.hpp>

export module ddge.modules.exec.scheduler.SignalTree;

namespace ddge::exec {

using NodeIndex = uint32_t;

using Counter = std::atomic<NodeIndex>;

static_assert(Counter::is_always_lock_free);

struct Node {
    Counter counter{};
};

export enum struct TravelsalBias : std::uint8_t {
    eLeft,
    eRight,
};

/**
 * Multi-producer multi-consumer
 */
export class SignalTree {
public:
    using LeafIndex       = NodeIndex;
    using TravelsalTactic = tl::function_ref<TravelsalBias(uint32_t current_node_index)>;

    constexpr static std::integral_constant<uint32_t, 2> minimum_number_of_levels;

    explicit SignalTree(uint32_t number_of_levels);

    auto try_set_one(LeafIndex leaf_index) -> bool;
    [[nodiscard]]
    auto try_unset_one(TravelsalTactic tactic) -> std::optional<LeafIndex>;
    [[nodiscard]]
    auto try_unset_one_at(LeafIndex leaf_index) -> bool;

    [[nodiscard]]
    auto full() const noexcept -> bool;

    [[nodiscard]]
    auto number_of_levels() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_leaves() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_branches() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_nodes() const noexcept -> uint32_t;

private:
    struct Precondition {
        explicit Precondition(uint32_t number_of_levels);
    };

    [[no_unique_address]]
    Precondition      m_precondition;
    std::vector<Node> m_nodes;

    auto set_branch(NodeIndex node_index) -> void;

    auto try_unset_one(
        NodeIndex                                 node_index,
        tl::function_ref<TravelsalBias(uint32_t)> strategy
    ) -> std::optional<LeafIndex>;
    auto try_unset_one_at(NodeIndex node_index, NodeIndex target_index) -> bool;
    auto try_unset_leaf(NodeIndex node_index) -> std::optional<LeafIndex>;

    [[nodiscard]]
    auto is_leaf_index(NodeIndex node_index) const noexcept -> bool;
};

}   // namespace ddge::exec
