module;

#include <cassert>
#include <cstdint>

#include <GLFW/glfw3.h>

export module ddge.modules.wsi.Context;

import ddge.modules.wsi.Error;

namespace ddge::wsi {

export class Context {
public:
    Context();
    Context(const Context&);
    Context(Context&&) noexcept = default;
    ~Context();

    auto operator=(const Context&) -> Context&     = default;
    auto operator=(Context&&) noexcept -> Context& = default;

private:
    inline static uint32_t active_context_count{};
};

}   // namespace ddge::wsi

module :private;

namespace ddge::wsi {

Context::Context()
{
    const bool success = glfwInit();

    if (!success) {
        const char* error_description{};
        const int   error_code = glfwGetError(&error_description);
        assert(
            (error_code == GLFW_PLATFORM_UNAVAILABLE || error_code == GLFW_PLATFORM_ERROR)
            && "Other error codes are unspecified"
        );

        throw Error{ error_description };
    }

    ++active_context_count;
}

Context::Context(const Context&)
{
    ++active_context_count;
}

Context::~Context()
{
    --active_context_count;

    if (active_context_count == 0) {
        glfwTerminate();

        const char* error_description{};
        const int   error_code = glfwGetError(&error_description);
        assert(
            (error_code == GLFW_NO_ERROR || error_code == GLFW_PLATFORM_ERROR)
            && "Other error codes are unspecified"
        );

        if (error_code == GLFW_PLATFORM_ERROR) {
            assert(
                false
                && "A bug or configuration error in GLFW,"
                   " the underlying operating system or its drivers,"
                   " or a lack of required resources "
                   "- Issue a ticket to GLFW"
            );
        }
    }
}

}   // namespace ddge::wsi
