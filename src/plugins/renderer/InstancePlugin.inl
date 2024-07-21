namespace plugins::renderer {

template <typename... Args>
auto InstancePlugin::emplace_dependency(Args&&... args) -> InstancePlugin&
{
    m_dependencies.emplace_back(std::forward<Args>(args)...);
    return *this;
}

}   // namespace plugins::renderer
