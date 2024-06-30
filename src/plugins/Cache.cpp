#include "Cache.hpp"

#include <spdlog/spdlog.h>

#include "core/cache/Cache.hpp"

namespace plugins {

auto Cache::operator()(App::Builder& t_builder) const noexcept -> void
{
    t_builder.store().emplace<core::cache::Cache>();

    SPDLOG_TRACE("Added Cache plugin");
}

}   // namespace plugins
