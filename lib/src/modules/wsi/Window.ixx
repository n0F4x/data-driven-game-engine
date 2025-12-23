module;

#include <cassert>
#include <memory>
#include <optional>
#include <variant>

#include <GLFW/glfw3.h>

#include "utility/contract_macros.hpp"

export module ddge.modules.wsi.Window;

import ddge.modules.wsi.Context;
import ddge.modules.wsi.Error;
import ddge.modules.wsi.monitors;
import ddge.modules.wsi.Resolution;
import ddge.utility.contracts;
import ddge.utility.Overloaded;

namespace ddge::wsi {

export struct WindowedScreenSettings {
    struct Border {
        bool resizable{ true };
    };

    enum struct Visibility {
        eDefault,
        eHidden,
        eFocused,
    };

    Resolution            resolution;
    std::optional<int>    position_x;
    std::optional<int>    position_y;
    std::optional<Border> border{ std::in_place };
    Visibility            visibility{ Visibility::eDefault };
    bool                  maximized{ false };
    bool                  focus_on_show{ true };
    bool                  scale_to_monitor{ false };
};

export struct FullScreenSettings {
    Monitor            monitor;
    bool               auto_iconify{ true };
    bool               center_cursor{ true };
    std::optional<int> refresh_rate;
};

export using ScreenSettings = std::variant<WindowedScreenSettings, FullScreenSettings>;

export class Window {
public:
    struct CreateInfo {
        const char*    title = "";
        ScreenSettings screen_settings;
    };

    explicit Window(Context&& context, const CreateInfo& create_info);

private:
    [[no_unique_address]]
    Context m_context;

    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_handle;
};

}   // namespace ddge::wsi

module :private;

namespace ddge::wsi {

auto destroy_window(GLFWwindow* const handle) -> void
{
    glfwDestroyWindow(handle);

    const int error_code = glfwGetError(nullptr);
    PRECOND(error_code != GLFW_NOT_INITIALIZED);
    assert(
        error_code != GLFW_PLATFORM_ERROR
        && "A bug or configuration error in GLFW,"
           " the underlying operating system or its drivers,"
           " or a lack of required resources "
           "- Issue a ticket to GLFW"
    );
    assert(error_code == GLFW_NO_ERROR && "Other error codes are unspecified");
}

[[nodiscard]]
auto create_window(const Window::CreateInfo& create_info)
    -> std::unique_ptr<GLFWwindow, decltype(&destroy_window)>
{
    const Resolution video_mode{ create_info.screen_settings.visit(
        util::Overloaded{
            [](const WindowedScreenSettings& settings) static -> Resolution {
                return settings.resolution;
            },
            [](const FullScreenSettings& settings) static -> Resolution {
                return settings.monitor.resolution();
            },
        }
    ) };

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (const WindowedScreenSettings* screen_settings{
            std::get_if<WindowedScreenSettings>(&create_info.screen_settings) };
        screen_settings != nullptr)
    {
        glfwWindowHint(
            GLFW_POSITION_X, screen_settings->position_x.value_or(GLFW_ANY_POSITION)
        );
        glfwWindowHint(
            GLFW_POSITION_Y, screen_settings->position_y.value_or(GLFW_ANY_POSITION)
        );

        const std::optional<WindowedScreenSettings::Border> border{
            screen_settings->border   //
        };
        glfwWindowHint(GLFW_DECORATED, border.has_value());
        if (border.has_value()) {
            glfwWindowHint(GLFW_RESIZABLE, border->resizable);
        }

        switch (screen_settings->visibility) {
            case WindowedScreenSettings::Visibility::eDefault:
                glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
                break;
            case WindowedScreenSettings::Visibility::eHidden:
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
                glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
                break;
            case WindowedScreenSettings::Visibility::eFocused:
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
                glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
                break;
        }

        glfwWindowHint(GLFW_MAXIMIZED, screen_settings->maximized);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, screen_settings->focus_on_show);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, screen_settings->scale_to_monitor);
    }
    if (const FullScreenSettings* screen_settings{
            std::get_if<FullScreenSettings>(&create_info.screen_settings) };
        screen_settings != nullptr)
    {
        glfwWindowHint(GLFW_AUTO_ICONIFY, screen_settings->auto_iconify);
        glfwWindowHint(GLFW_CENTER_CURSOR, screen_settings->center_cursor);
        glfwWindowHint(
            GLFW_REFRESH_RATE, screen_settings->refresh_rate.value_or(GLFW_DONT_CARE)
        );
    }

    GLFWwindow* const window = glfwCreateWindow(
        video_mode.width,
        video_mode.height,
        create_info.title,
        create_info.screen_settings.visit(
            util::Overloaded{
                [](const WindowedScreenSettings&) static -> GLFWmonitor* {
                    return nullptr;
                },
                [](const FullScreenSettings& settings) static -> GLFWmonitor* {
                    return static_cast<GLFWmonitor*>(settings.monitor);
                },
            }
        ),
        nullptr
    );

    if (window == nullptr) {
        const char* error_description{};
        const int   error_code = glfwGetError(&error_description);
        assert(error_code != GLFW_NO_ERROR);
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        PRECOND(error_code != GLFW_INVALID_ENUM);
        PRECOND(error_code != GLFW_INVALID_VALUE);
        PRECOND(error_code != GLFW_API_UNAVAILABLE && "Vulkan support is not available");
        PRECOND(error_code != GLFW_VERSION_UNAVAILABLE);
        PRECOND(error_code != GLFW_FORMAT_UNAVAILABLE);
        PRECOND(error_code != GLFW_NO_WINDOW_CONTEXT);
        assert(error_code == GLFW_PLATFORM_ERROR && "Other error codes are unspecified");

        throw Error{ error_description };
    }

    return std::unique_ptr<GLFWwindow, decltype(&destroy_window)>{
        window,
        destroy_window   //
    };
}

Window::Window(Context&& context, const CreateInfo& create_info)
    : m_context{ std::move(context) },
      m_handle{ create_window(create_info) }
{}

}   // namespace ddge::wsi
