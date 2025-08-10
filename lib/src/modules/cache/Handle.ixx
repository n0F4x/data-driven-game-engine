module;

#include <memory>

export module ddge.modules.cache.Handle;

namespace ddge::cache {

export template <typename Resource>
using Handle = std::shared_ptr<Resource>;

export template <typename Resource>
[[nodiscard]]
auto make_handle(auto&&... args) -> Handle<Resource>;

}   // namespace ddge::cache

namespace ddge::cache {

template <typename Resource>
auto make_handle(auto&&... args) -> Handle<Resource>
{
    return std::make_shared<Resource>(std::forward<decltype(args)>(args)...);
}

}   // namespace ddge::cache
