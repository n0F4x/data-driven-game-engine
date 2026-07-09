module;

#include <memory>
#include <memory_resource>

export module ddge.app.v2.memory.MemoryArena;

import ddge.registry.ConfigurationEntry;

namespace ddge::app::v2 {

export class MemoryArena : public registry::ConfigurationEntry {
public:
    class Builder;

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto pool_allocator() -> std::pmr::polymorphic_allocator<>
    {
        return m_pool_resource.get();
    }

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto make_transient_resource() -> std::pmr::monotonic_buffer_resource
    {
        return std::pmr::monotonic_buffer_resource{ m_transient_memory_resource.get() };
    }

private:
    std::shared_ptr<std::pmr::monotonic_buffer_resource> m_monotonic_resource{
        std::make_shared<std::pmr::monotonic_buffer_resource>()
    };
    std::shared_ptr<std::pmr::unsynchronized_pool_resource> m_pool_resource{
        std::make_shared<std::pmr::unsynchronized_pool_resource>(
            m_monotonic_resource.get()
        )
    };
    std::shared_ptr<std::pmr::unsynchronized_pool_resource> m_transient_memory_resource{
        std::make_shared<std::pmr::unsynchronized_pool_resource>(
            m_monotonic_resource.get()
        )
    };
};

}   // namespace ddge::app::v2
