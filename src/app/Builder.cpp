#include "App.hpp"

auto App::Builder::build() && noexcept -> App
{
    return App{ std::move(*this).m_store };
}
