#include "App.hpp"

#include <spdlog/spdlog.h>

#include "Builder.hpp"

namespace engine {

//////////////////////////////
///----------------------- ///
///  App   IMPLEMENTATION  ///
///------------------------///
//////////////////////////////
App::App(Store&& t_store) noexcept : m_store{ std::move(t_store) } {}

auto App::create() noexcept -> App::Builder
{
    SPDLOG_TRACE("Creating App...");
    return Builder{};
}

auto App::store() noexcept -> App::Store&
{
    return m_store;
}

}   // namespace engine
