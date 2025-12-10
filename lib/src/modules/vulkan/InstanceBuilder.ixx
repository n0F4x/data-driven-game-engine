module;

#include <cstdint>
#include <vector>

export module ddge.modules.vulkan.InstanceBuilder;

import vulkan_hpp;

import ddge.modules.vulkan.minimum_vulkan_api_version;

namespace ddge::vulkan {

struct InstanceBuilderPrecondition {
    explicit InstanceBuilderPrecondition(const vk::raii::Context& context);
};

export class InstanceBuilder : InstanceBuilderPrecondition {
public:
    struct CreateInfo {
        const char* application_name{};
        uint32_t    application_version{};
        const char* engine_name{};
        uint32_t    engine_version{};
    };

    InstanceBuilder(const CreateInfo& create_info, const vk::raii::Context& context);

    auto request_vulkan_api_version(uint32_t vulkan_api_version) -> void;
    [[nodiscard]]
    auto enable_vulkan_layer(const char* layer_name) -> bool;
    [[nodiscard]]
    auto enable_extension(const char* extension_name) -> bool;

    [[nodiscard]]
    auto build() && -> vk::raii::Instance;

private:
    std::reference_wrapper<const vk::raii::Context> m_context;
    const char*                                     m_application_name{};
    uint32_t                                        m_application_version{};
    const char*                                     m_engine_name{};
    uint32_t                                        m_engine_version{};
    uint32_t                 m_vulkan_api_version{ minimum_vulkan_api_version() };
    std::vector<const char*> m_layer_names;
    std::vector<const char*> m_extension_names;
};

}   // namespace ddge::vulkan
