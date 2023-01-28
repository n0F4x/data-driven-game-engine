#pragma once

#include <functional>
#include <ranges>
#include <vector>

#include "common/patterns/builder/helper.hpp"

namespace engine {

class Controller;

class Stage final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using System = std::function<void(Controller&)>;

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend BuilderBase<Stage>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto empty() const noexcept -> bool;
    void run(Controller& t_controller) const;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::vector<System> m_systems;
};

class Stage::Builder : public BuilderBase<Stage> {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    using BuilderBase<Stage>::BuilderBase;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto add_system(Stage::System&& t_system) -> Builder&;
};

}   // namespace engine
