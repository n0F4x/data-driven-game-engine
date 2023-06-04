#pragma once

#include <string_view>

namespace simple_app {

template <class AppType>
class AppView final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using App = AppType;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit(false) AppView(App& t_app);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto name() -> std::string_view;
    [[nodiscard]] auto renderer() -> App::Renderer&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    App& m_app;
};

}   // namespace simple_app

#include "AppView.inl"
