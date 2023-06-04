namespace engine {

Controller::Controller(Schedule& t_schedule) noexcept
    : m_schedule{ t_schedule } {}

void Controller::quit() noexcept {
    m_schedule.quit();
}

}   // namespace engine
