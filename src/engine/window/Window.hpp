#pragma once

#include <SFML/Window.hpp>
#include <SFML/Window/Vulkan.hpp>

namespace engine {

class Window {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend Builder;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Window(
        const sf::VideoMode& t_video_mode,
        const sf::String&    t_title,
        sf::Uint32           t_style
    );

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] sf::Vector2u getSize() const noexcept;

    bool createVulkanSurface(
        const VkInstance&            t_instance,
        VkSurfaceKHR&                t_surface,
        const VkAllocationCallbacks* t_allocator = nullptr
    );

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    sf::WindowBase m_pimpl;
};

class Window::Builder {
    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend Window;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Builder() = default;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() const noexcept(false) -> Window;

    auto set_style(sf::Uint32 t_style) noexcept -> Builder&;
    auto set_title(const sf::String& t_title) noexcept -> Builder&;
    auto set_video_mode(const sf::VideoMode& t_video_mode) noexcept -> Builder&;

    [[nodiscard]] auto style() const noexcept -> sf::Uint32;
    [[nodiscard]] auto title() const noexcept -> const sf::String&;
    [[nodiscard]] auto video_mode() const noexcept -> const sf::VideoMode&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    sf::Uint32    m_style = sf::Style::Default;
    sf::String    m_title;
    sf::VideoMode m_video_mode;
};

}   // namespace engine
