#pragma once

#include <concepts>
#include <expected>
#include <span>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

class Instance {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Instance(Instance&&) noexcept;
    ~Instance() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Instance&&) noexcept -> Instance& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Instance;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Instance*;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Builder;

    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Instance m_instance;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Instance(vk::Instance t_instance) noexcept;

};

class Instance::Builder {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Error {
        std::vector<const char*> unsupported_layers{};
        std::vector<const char*> unsupported_extensions{};
        vk::Result               code;
    };

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() const noexcept -> std::expected<Instance, Error>;

    auto set_application_name(std::string_view t_application_name) noexcept
        -> Builder&;
    auto set_application_version(uint32_t t_application_version) noexcept
        -> Builder&;
    auto set_engine_name(std::string_view t_engine_name) noexcept -> Builder&;
    auto set_engine_version(uint32_t t_engine_version) noexcept -> Builder&;
    auto set_api_version(uint32_t t_api_version) noexcept -> Builder&;

    auto add_layers(std::span<const char* const> t_layers) noexcept -> Builder&;
    auto add_extensions(std::span<const char* const> t_extensions) noexcept
        -> Builder&;
    auto add_layer(const char* t_layer) noexcept -> Builder&;
    auto add_extension(const char* t_extension) noexcept -> Builder&;

    [[nodiscard]] auto layers() const noexcept
        -> const std::vector<const char*>&;
    [[nodiscard]] auto extensions() const noexcept
        -> const std::vector<const char*>&;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Instance;

    ///*************///
    ///  Variables  ///
    ///*************///
    std::string_view         m_application_name{};
    uint32_t                 m_application_version = VK_API_VERSION_1_0;
    std::string_view         m_engine_name{};
    uint32_t                 m_engine_version = VK_API_VERSION_1_0;
    uint32_t                 m_api_version    = VK_API_VERSION_1_0;
    std::vector<const char*> m_layers{};
    std::vector<const char*> m_extensions{};

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Builder() noexcept = default;
};

}   // namespace engine::vulkan
