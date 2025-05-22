module;

#include <memory>

export module core.cache.Handle;

namespace core::cache {

export template <typename Resource>
using Handle = std::shared_ptr<Resource>;

export template <typename Resource>
[[nodiscard]]
auto make_handle(auto&&... args) -> Handle<Resource>;

}   // namespace core::cache

namespace core::cache {

template <typename Resource>
auto make_handle(auto&&... args) -> Handle<Resource>
{
    return std::make_shared<Resource>(std::forward<decltype(args)>(args)...);
}

}   // namespace core::cache
