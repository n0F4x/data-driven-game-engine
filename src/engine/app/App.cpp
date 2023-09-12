#include "App.hpp"

#include "Builder.hpp"

namespace engine {

//////////////////////////////
///----------------------- ///
///  App   IMPLEMENTATION  ///
///------------------------///
//////////////////////////////
App::App(Context&& t_context) noexcept : m_context{ std::move(t_context) } {}

auto App::create() noexcept -> App::Builder
{
    return Builder{};
}

auto App::context() noexcept -> App::Context&
{
    return m_context;
}

}   // namespace engine
