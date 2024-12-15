module;

#include <any>

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

export module core.app.App;

import utility.meta.functional;
import utility.meta.tuple_like;
import utility.tuple;

import core.store.Store;

namespace core::app {

export class App {
public:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Store          resources;
    entt::registry registry;
};

}   // namespace core::app
