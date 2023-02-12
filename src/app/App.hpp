#pragma once

#include <string>
#include <memory>

#include "AppView.hpp"
#include "config.hpp"

namespace app {

template <engine::RendererConcept RendererType>
class BasicApp final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using AppView = AppView<BasicApp<RendererType>>;
    using Renderer = RendererType;
    using ScheduleHandle = std::unique_ptr<ScheduleInterface<AppView>>;

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend AppView;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicApp(std::string_view t_name,
                                    Renderer&& t_renderer,
                                    ScheduleHandle&& t_schedule) noexcept;

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
    ScheduleHandle m_schedule;
};

template <engine::RendererConcept RendererType>
class BasicApp<RendererType>::Builder final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicApp<RendererType>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product() noexcept;
    [[nodiscard]] auto build() noexcept -> Product;

    [[nodiscard]] auto set_name(std::string_view t_name) noexcept -> Builder&;
    [[nodiscard]] auto set_renderer(Renderer&& t_renderer) -> Builder&;
    [[nodiscard]] auto set_schedule(ScheduleHandle&& t_schedule) -> Builder&;
    template <class Schedule, typename... Args>
    [[nodiscard]] auto set_schedule(Args&&... t_args) -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string_view m_name = "App";
    Renderer m_renderer;
    ScheduleHandle m_schedule = std::make_unique<config::Schedule<AppView>>();
};

using App = BasicApp<config::Renderer>;

}   // namespace app

#include "App.inl"
