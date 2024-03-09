#include "Builder.hpp"

namespace engine {

auto App::Builder::store() noexcept -> Store&
{
    return m_store;
}

auto App::Builder::store() const noexcept -> const Store&
{
    return m_store;
}

auto App::Builder::build() && noexcept -> App
{
    return App{ std::move(m_store) };
}

}   // namespace engine
