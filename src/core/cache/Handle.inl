#pragma once

namespace core::cache {

template <typename Resource>
auto make_handle(auto&&... args) -> Handle<Resource>
{
    return std::make_shared<Resource>(std::forward<decltype(args)>(args)...);
}

}   // namespace core::cache
