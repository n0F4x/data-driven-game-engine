#pragma once

#include <memory>

#include <tl/optional.hpp>

#include <SFML/Window/WindowBase.hpp>

#include "Style.hpp"

namespace engine::window {

class Window {
public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        const sf::VideoMode& t_video_mode,
        const sf::String&    t_title,
        Style                t_style
    ) noexcept -> tl::optional<Window>;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator->() const noexcept -> sf::WindowBase*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto framebuffer_size() const noexcept -> sf::Vector2u;

    [[nodiscard]] auto create_vulkan_surface(
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
