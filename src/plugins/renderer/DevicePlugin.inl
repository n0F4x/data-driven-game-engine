namespace plugins::renderer {

template <typename... Args>
auto DevicePlugin::emplace_dependency(Args&&... args) -> DevicePlugin&
{
    m_dependencies.emplace_back(std::forward<Args>(args)...);
    return *this;
}

}   // namespace plugins::renderer
