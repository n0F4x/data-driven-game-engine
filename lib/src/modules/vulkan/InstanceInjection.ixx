module;

#include <utility>

export module ddge.modules.vulkan.InstanceInjection;

import vulkan_hpp;

import ddge.modules.vulkan.InstanceBuilder;

namespace ddge::vulkan {

export class InstanceInjection {
public:
    explicit InstanceInjection(vulkan::InstanceBuilder&& instance_builder)
        : m_instance_builder{ std::move(instance_builder) }
    {}

    auto operator->() noexcept -> vulkan::InstanceBuilder*
    {
        return &m_instance_builder;
    }

    auto operator->() const noexcept -> const vulkan::InstanceBuilder*
    {
        return &m_instance_builder;
    }

    auto operator*() noexcept -> vulkan::InstanceBuilder&
    {
        return m_instance_builder;
    }

    auto operator*() const noexcept -> const vulkan::InstanceBuilder&
    {
        return m_instance_builder;
    }

    [[nodiscard]]
    auto operator()() const -> vk::raii::Instance
    {
        return m_instance_builder.build();
    }

private:
    vulkan::InstanceBuilder m_instance_builder;
};

}   // namespace ddge::vulkan
