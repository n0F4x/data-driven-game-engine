namespace app {

template <class ScheduleType>
Controller<ScheduleType>::Controller(Schedule& t_schedule) noexcept
    : m_schedule{ t_schedule } {}

template <class ScheduleType>
auto Controller<ScheduleType>::running() const noexcept -> bool {
    return m_schedule.running();
}

template <class ScheduleType>
void Controller<ScheduleType>::quit() noexcept {
    m_schedule.quit();
}

}   // namespace app
