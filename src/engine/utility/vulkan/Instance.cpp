#include "Instance.hpp"

namespace engine::vulkan {

///////////////////////////////////
///-----------------------------///
///  Instance   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Instance::Instance(vk::Instance t_instance) noexcept : m_instance{ t_instance }
{}

Instance::Instance(Instance&& t_other) noexcept
    : m_instance{ t_other.m_instance }
{
    t_other.m_instance = nullptr;
}

Instance::~Instance() noexcept
{
    if (m_instance) {
        m_instance.destroy();
    }
}

auto Instance::operator*() const noexcept -> vk::Instance
{
    return m_instance;
}

auto Instance::operator->() const noexcept -> const vk::Instance*
{
    return &m_instance;
}

auto Instance::create() noexcept -> Builder
{
    return Builder{};
}

}   // namespace engine::vulkan

namespace {

auto find_invalid_layers(const std::vector<const char*>& t_layers)
    -> std::expected<std::vector<const char*>, vk::Result>
{
    auto [result, available_layers]{ vk::enumerateInstanceLayerProperties() };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    std::vector<const char*> invalid_layers;

    for (auto validationLayer : t_layers) {
        if (std::ranges::find_if(
                available_layers,
                [validationLayer](auto availableLayer) {
                    return strcmp(validationLayer, availableLayer) == 0;
                },
                &vk::LayerProperties::layerName
            )
            == available_layers.end())
        {
            invalid_layers.push_back(validationLayer);
        }
    }

    return invalid_layers;
}

auto find_invalid_extensions(const std::vector<const char*>& t_extensions)
    -> std::expected<std::vector<const char*>, vk::Result>
{
    auto [result, available_extensions]{
        vk::enumerateInstanceExtensionProperties()
    };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    std::vector<const char*> invalid_extensions;

    for (auto extension : t_extensions) {
        if (std::ranges::find_if(
                available_extensions,
                [extension](auto availableLayer) {
                    return strcmp(extension, availableLayer) == 0;
                },
                &vk::ExtensionProperties::extensionName
            )
            == available_extensions.end())
        {
            invalid_extensions.push_back(extension);
        }
    }

    return invalid_extensions;
}

auto check_for_error(const engine::vulkan::Instance::Builder& t_builder)
    -> std::optional<engine::vulkan::Instance::Builder::Error>
{
    engine::vulkan::Instance::Builder::Error error{};

    if (auto result{ find_invalid_layers(t_builder.layers()) }) {
        error.unsupported_layers = std::move(*result);
    }
    else {
        error.code = result.error();
    }

    if (auto result{ find_invalid_extensions(t_builder.extensions()) }) {
        error.unsupported_extensions = std::move(*result);
    }
    else {
        error.code = result.error();
    }

    if (error.code != vk::Result::eSuccess || !error.unsupported_layers.empty()
        || !error.unsupported_extensions.empty())
    {
        return error;
    }
    return std::nullopt;
}

}   // namespace

namespace engine::vulkan {

////////////////////////////////////////////
///--------------------------------------///
///  Instance::Builder   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////

auto Instance::Builder::build() const noexcept -> std::expected<Instance, Error>
{
    if (auto result{ check_for_error(*this) }; result.has_value()) {
        return std::unexpected{ *result };
    }

    vk::ApplicationInfo    applicationInfo{ .pApplicationName =
                                             m_application_name.data(),
                                            .applicationVersion =
                                             m_application_version,
                                            .pEngineName = m_engine_name.data(),
                                            .engineVersion = m_engine_version,
                                            .apiVersion    = m_api_version };
    vk::InstanceCreateInfo instanceCreateInfo{
        .pApplicationInfo        = &applicationInfo,
        .enabledLayerCount       = static_cast<uint32_t>(m_layers.size()),
        .ppEnabledLayerNames     = m_layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(m_extensions.size()),
        .ppEnabledExtensionNames = m_extensions.data()
    };
    if (auto [result, instance]{ vk::createInstance(instanceCreateInfo) };
        result == vk::Result::eSuccess)
    {
        return Instance{ instance };
    }
    else {
        return std::unexpected{ Error{ .code = result } };
    }
}

auto Instance::Builder::set_application_name(std::string_view t_application_name
) noexcept -> Builder&
{
    m_application_name = t_application_name;
    return *this;
}

auto Instance::Builder::set_application_version(uint32_t t_application_version
) noexcept -> Builder&
{
    m_application_version = t_application_version;
    return *this;
}

auto Instance::Builder::set_engine_name(std::string_view t_engine_name) noexcept
    -> Builder&
{
    m_engine_name = t_engine_name;
    return *this;
}

auto Instance::Builder::set_engine_version(uint32_t t_engine_version) noexcept
    -> Builder&
{
    m_engine_version = t_engine_version;
    return *this;
}

auto Instance::Builder::set_api_version(uint32_t t_api_version) noexcept
    -> Builder&
{
    m_api_version = t_api_version;
    return *this;
}

auto Instance::Builder::add_layers(std::span<const char* const> t_layers) noexcept
    -> Builder&
{
    m_layers.insert(m_layers.end(), t_layers.begin(), t_layers.end());
    return *this;
}

auto Instance::Builder::add_extensions(std::span<const char* const> t_extensions
) noexcept -> Builder&
{
    m_extensions.insert(
        m_extensions.end(), t_extensions.begin(), t_extensions.end()
    );
    return *this;
}

auto Instance::Builder::add_layer(const char* t_layer) noexcept
    -> Instance::Builder&
{
    m_layers.push_back(t_layer);
    return *this;
}

auto Instance::Builder::add_extension(const char* t_extension) noexcept
    -> Instance::Builder&
{
    m_extensions.push_back(t_extension);
    return *this;
}

auto Instance::Builder::layers() const noexcept
    -> const std::vector<const char*>&
{
    return m_layers;
}

auto Instance::Builder::extensions() const noexcept
    -> const std::vector<const char*>&
{
    return m_extensions;
}

}   // namespace engine::vulkan
