module;

#include <cassert>

#include <GLFW/glfw3.h>

#include "util/contract_macros.hpp"

export module ddge.modules.wsi.events.poll_events;

import ddge.modules.wsi.Context;
import ddge.modules.wsi.Error;
import ddge.util.contracts;

namespace ddge::wsi {

export auto poll_events(const Context&) -> void
{
    glfwPollEvents();

    const char* error_description{};
    const int   error_code = glfwGetError(&error_description);
    if (error_code != GLFW_NO_ERROR) {
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        assert(error_code == GLFW_PLATFORM_ERROR && "Other error codes are unspecified");
        throw Error{ error_description };
    }
}

}   // namespace ddge::wsi
