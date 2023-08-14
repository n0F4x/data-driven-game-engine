#pragma once

#include <memory>
#include <optional>

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
    explicit Window(
        const sf::VideoMode& t_video_mode,
        const sf::String&    t_title,
        sf::Uint32           t_style
    ) noexcept(false);

public:
    Window(Window&&) noexcept = default;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Window&&) noexcept -> Window& = default;
    [[nodiscard]] auto operator*() const noexcept -> sf::WindowBase&;
    [[nodiscard]] auto operator->() const noexcept -> sf::WindowBase*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto getSize() const noexcept -> sf::Vector2u;

    auto createVulkanSurface(
        VkInstance                   t_instance,
        const VkAllocationCallbacks* t_allocator = nullptr
    ) noexcept -> std::optional<VkSurfaceKHR>;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::unique_ptr<sf::WindowBase> m_impl;
};

class Window::Builder {
    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend Window;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Builder() noexcept = default;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() const noexcept -> std::optional<Window>;

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