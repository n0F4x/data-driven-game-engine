#pragma once

#include "App.hpp"

namespace engine {

class Plugin {
public:
    virtual auto set_context(App::Context& t_app_context) const -> void = 0;
};

}   // namespace engine
