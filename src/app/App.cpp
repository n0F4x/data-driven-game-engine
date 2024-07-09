#include "App.hpp"

#include <spdlog/spdlog.h>

App::App(Store&& store) : m_store{ std::move(store) } {}

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
