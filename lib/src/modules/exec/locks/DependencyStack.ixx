module;

#include <span>
#include <vector>

export module ddge.modules.exec.locks.DependencyStack;

import ddge.modules.exec.locks.LockGroup;
import ddge.modules.exec.locks.LockOwnerIndex;
import ddge.modules.exec.locks.TopOfLockStack;

namespace ddge::exec {

export class DependencyStack {
public:
    auto emplace(const LockGroup& lock_group) -> LockOwnerIndex;

    [[nodiscard]]
    auto bottom() const noexcept -> std::span<const LockOwnerIndex>;
    [[nodiscard]]
    auto size() const noexcept -> LockOwnerIndex;
    [[nodiscard]]
    auto dependencies_of(LockOwnerIndex index) const -> std::span<const LockOwnerIndex>;
    [[nodiscard]]
    auto dependees_of(LockOwnerIndex index) const -> std::span<const LockOwnerIndex>;
    [[nodiscard]]
    auto locks() const noexcept -> const LockGroup&;

private:
    std::vector<LockOwnerIndex>              m_bottom;
    TopOfLockStack                           m_top_locks;
    std::vector<std::vector<LockOwnerIndex>> m_dependencies;
    std::vector<std::vector<LockOwnerIndex>> m_dependees;
    LockGroup                                m_locks;
};

}   // namespace ddge::exec

auto ddge::exec::DependencyStack::emplace(const LockGroup& lock_group) -> LockOwnerIndex
{
    m_dependencies.push_back(lock_group.dependencies(m_top_locks));
    const LockOwnerIndex index{ static_cast<LockOwnerIndex>(m_dependencies.size() - 1) };

    for (const LockOwnerIndex dependency_index : m_dependencies[index]) {
        m_dependees[dependency_index].push_back(index);
    }
    m_dependees.emplace_back();

    m_top_locks = lock_group.transform_top_of_lock_stack(std::move(m_top_locks), index);

    if (m_dependencies[index].empty()) {
        m_bottom.push_back(index);
    }

    m_locks.expand(lock_group);

    return index;
}

auto ddge::exec::DependencyStack::bottom() const noexcept
    -> std::span<const LockOwnerIndex>
{
    return m_bottom;
}

auto ddge::exec::DependencyStack::size() const noexcept -> LockOwnerIndex
{
    return static_cast<LockOwnerIndex>(m_dependencies.size());
}

auto ddge::exec::DependencyStack::dependencies_of(const LockOwnerIndex index) const
    -> std::span<const LockOwnerIndex>
{
    return m_dependencies[index];
}

auto ddge::exec::DependencyStack::dependees_of(const LockOwnerIndex index) const
    -> std::span<const LockOwnerIndex>
{
    return m_dependees[index];
}

auto ddge::exec::DependencyStack::locks() const noexcept -> const LockGroup&
{
    return m_locks;
}
