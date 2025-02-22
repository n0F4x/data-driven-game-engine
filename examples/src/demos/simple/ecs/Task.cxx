module ecs;

auto ecs::Task::operator()() const -> void
{
    m_invocation();
}
