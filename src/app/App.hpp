#pragma once

#include <string>

#include "AppView.hpp"
#include "config.hpp"

namespace app {

template <class RendererType, template<class> class ScheduleType>
class BasicApp final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using AppView = AppView<BasicApp<RendererType, ScheduleType>>;
    using Renderer = RendererType;
    using Schedule = ScheduleType<AppView>;

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend AppView;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicApp(std::string_view t_name,
                                    Renderer&& t_renderer,
                                    Schedule&& t_schedule) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run();

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string m_name;
    Renderer m_renderer;
    Schedule m_schedule;
};

template <class RendererType, template<class> class ScheduleType>
class BasicApp<RendererType, ScheduleType>::Builder final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicApp<RendererType, ScheduleType>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product() noexcept;
    [[nodiscard]] auto build() noexcept -> Product;

    [[nodiscard]] auto set_name(std::string_view t_name) noexcept -> Builder&;
    [[nodiscard]] auto set_renderer(Renderer&& t_renderer) -> Builder&;
    [[nodiscard]] auto set_schedule(Schedule&& t_schedule) -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string_view m_name = "App";
    Renderer m_renderer;
    Schedule m_schedule;
};

using App = BasicApp<config::Renderer, config::Schedule>;

}   // namespace app

#include "App.inl"
