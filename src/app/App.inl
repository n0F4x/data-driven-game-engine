#include <algorithm>
#include <iostream>

namespace app {

///////////////////////////////////
///-----------------------------///
///  BasicApp   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////
template <engine::RendererConcept RendererType>
BasicApp<RendererType>::BasicApp(std::string_view t_name,
                                 Renderer&& t_renderer,
                                 ScheduleHandle&& t_schedule) noexcept
    : m_name{ t_name },
      m_renderer{ std::move(t_renderer) },
      m_schedule{ std::move(t_schedule) } {}

template <engine::RendererConcept RendererType>
void BasicApp<RendererType>::run() {
    m_schedule->execute(*this);
}

////////////////////////////////////////////
///--------------------------------------///
///  BasicApp::Builder   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////
template <engine::RendererConcept RendererType>
BasicApp<RendererType>::Builder::operator Product() noexcept {
    return build();
}

template <engine::RendererConcept RendererType>
auto BasicApp<RendererType>::Builder::build() noexcept -> Product {
    return BasicApp{ m_name, std::move(m_renderer), std::move(m_schedule) };
}

template <engine::RendererConcept RendererType>
auto BasicApp<RendererType>::Builder::set_name(std::string_view t_name) noexcept
    -> Builder& {
    m_name = t_name;
    return *this;
}

template <engine::RendererConcept RendererType>
auto BasicApp<RendererType>::Builder::set_renderer(Renderer&& t_renderer)
    -> Builder& {
    m_renderer = std::move(t_renderer);
    return *this;
}

template <engine::RendererConcept RendererType>
auto BasicApp<RendererType>::Builder::set_schedule(ScheduleHandle&& t_schedule)
    -> Builder& {
    m_schedule = std::move(t_schedule);
    return *this;
}

template <engine::RendererConcept RendererType>
template <class Schedule, typename... Args>
auto BasicApp<RendererType>::Builder::set_schedule(Args&&... t_args)
    -> Builder& {
    m_schedule = std::make_unique<Schedule>(t_args...);
    return *this;
}

}   // namespace app
