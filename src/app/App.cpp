#include "App.hpp"

#include <spdlog/spdlog.h>

App::App(Builder&& t_builder) : m_store{ std::move(t_builder.store()) } {}

auto App::create() -> Builder
{
    SPDLOG_TRACE("Creating App...");
    return Builder{};
}

auto App::store() noexcept -> Store&
{
    return m_store;
}

auto App::store() const noexcept -> const Store&
{
    return m_store;
}
