#include <spdlog/spdlog.h>

#include "app/core/Builder.hpp"
#include "engine/common/Cache.hpp"

#include "Common.hpp"

namespace plugins {

auto Common::operator()(app::App::Builder& t_builder) noexcept -> void
{
    t_builder.store().emplace<engine::Cache>();

    SPDLOG_TRACE("Added Common plugin");
}

}   // namespace engine::common
