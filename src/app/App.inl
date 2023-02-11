#include <algorithm>
#include <iostream>

namespace app {

///////////////////////////////////
///-----------------------------///
///  BasicApp   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////
template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
                 ScheduleType,
                 AppView<internal::MockApp<RendererType, ScheduleType>>>
BasicApp<RendererType, ScheduleType>::BasicApp(std::string_view t_name,
                                               Renderer&& t_renderer,
                                               Schedule&& t_schedule) noexcept
    : m_name{ t_name },
      m_renderer{ std::move(t_renderer) },
      m_schedule{ std::move(t_schedule) } {}

template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
        ScheduleType,
        AppView<internal::MockApp<RendererType, ScheduleType>>>
void BasicApp<RendererType, ScheduleType>::run() {
    m_schedule.execute(*this);
}

////////////////////////////////////////////
///--------------------------------------///
///  BasicApp::Builder   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////
template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
        ScheduleType,
        AppView<internal::MockApp<RendererType, ScheduleType>>>
BasicApp<RendererType, ScheduleType>::Builder::operator Product() noexcept {
    return build();
}

template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
        ScheduleType,
        AppView<internal::MockApp<RendererType, ScheduleType>>>
auto BasicApp<RendererType, ScheduleType>::Builder::build() noexcept
    -> Product {
    return BasicApp{ m_name, std::move(m_renderer), std::move(m_schedule) };
}

template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
        ScheduleType,
        AppView<internal::MockApp<RendererType, ScheduleType>>>
auto BasicApp<RendererType, ScheduleType>::Builder::set_name(
    std::string_view t_name) noexcept -> Builder& {
    m_name = t_name;
    return *this;
}

template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
        ScheduleType,
        AppView<internal::MockApp<RendererType, ScheduleType>>>
auto BasicApp<RendererType, ScheduleType>::Builder::set_renderer(
    Renderer&& t_renderer) -> Builder& {
    m_renderer = std::move(t_renderer);
    return *this;
}

template <engine::RendererConcept RendererType,
          template <class>
          class ScheduleType>
    requires ScheduleConcept<
        ScheduleType,
        AppView<internal::MockApp<RendererType, ScheduleType>>>
auto BasicApp<RendererType, ScheduleType>::Builder::set_schedule(
    Schedule&& t_schedule) -> Builder& {
    m_schedule = std::move(t_schedule);
    return *this;
}

}   // namespace app
