module ecs;

auto ecs::System::operator()() const -> void
{
    m_invocation();
}
