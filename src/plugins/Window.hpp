#pragma once

#include <string>

#include "app.hpp"

namespace plugins {

class Window {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(Store& store, uint16_t width, uint16_t height, const std::string& title)
        const -> void;

    auto operator()(
        Store&              store,
        uint16_t            width,
        uint16_t            height,
        const std::string&  title,
        std::invocable auto configure
    ) const -> void;
};

}   // namespace plugins

#include "Window.inl"
