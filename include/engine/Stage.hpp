#pragma once

#include <functional>
#include <ranges>
#include <vector>

#include "common/patterns/builder/helper.hpp"

namespace engine {

class Controller;

class Stage final {
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using System = std::function<void(Controller&)>;

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend BuilderBase<Stage>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] Stage(Stage&&) noexcept = default;

private:
    [[nodiscard]] Stage() noexcept = default;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto empty() const noexcept -> bool;
    void run(Controller& t_controller) const;

    ///--------------------///
    ///  Static functions  ///
    ///--------------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

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
