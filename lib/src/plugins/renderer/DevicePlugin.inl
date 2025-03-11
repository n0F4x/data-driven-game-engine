#pragma once

template <typename Self>
auto plugins::renderer::DevicePlugin::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(dependency);
    return std::forward<Self>(self);
}
