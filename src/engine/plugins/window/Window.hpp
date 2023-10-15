#pragma once

#include <memory>
#include <optional>

#include <SFML/Window.hpp>
#include <SFML/Window/Vulkan.hpp>

namespace engine::window {

class Window {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    enum class Style {
        eNone       = 0,
        eTitleBar   = 1 << 0,
        eResize     = 1 << 1,
        eClose      = 1 << 2,
        eFullscreen = 1 << 3,
        eDefault    = eTitleBar | eResize | eClose
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        const sf::VideoMode& t_video_mode,
        const sf::String&    t_title,
        Style                t_style
    ) noexcept -> std::optional<Window>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto framebuffer_size() const noexcept -> sf::Vector2u;

    auto create_vulkan_surface(
        VkInstance                   t_instance,
        const VkAllocationCallbacks* t_allocator = nullptr
    ) noexcept -> VkSurfaceKHR;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    std::unique_ptr<sf::WindowBase> m_impl;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Window(
        const sf::VideoMode& t_video_mode,
        const sf::String&    t_title,
        sf::Uint32           t_style
    ) noexcept(false);
};

}   // namespace engine::window
