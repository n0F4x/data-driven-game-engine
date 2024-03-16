#include "Common.hpp"

#include <spdlog/spdlog.h>

#include "app/Builder.hpp"
#include "core/common/Cache.hpp"

namespace plugins {

auto Common::operator()(app::App::Builder& t_builder) noexcept -> void
{
    t_builder.store().emplace<core::Cache>();

    SPDLOG_TRACE("Added Common plugin");
}

}   // namespace plugins
