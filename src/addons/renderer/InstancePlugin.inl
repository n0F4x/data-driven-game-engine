#pragma once

template <typename Self>
auto addons::renderer::InstancePlugin::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(std::move(dependency));
    return std::forward<Self>(self);
}
