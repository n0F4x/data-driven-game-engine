#include "Builder.hpp"

namespace engine {

///////////////////////////////////////
///---------------------------------///
///  App::Builder   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////
auto App::Builder::build() && noexcept -> App
{
    return App{ std::move(m_store) };
}

}   // namespace engine
