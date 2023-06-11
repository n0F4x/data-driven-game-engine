#pragma once

#include "SFML/Window.hpp"
#include "SFML/Window/Vulkan.hpp"

namespace engine {

class Window : public sf::WindowBase {
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
    using sf::WindowBase::WindowBase;
    [[nodiscard]] explicit Window(const Builder& t_builder);
};

class Window::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() const -> Window;

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
