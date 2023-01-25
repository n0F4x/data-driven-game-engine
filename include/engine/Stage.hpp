#pragma once

#include <functional>
#include <ranges>
#include <vector>

#include "common/patterns/builder/helper.hpp"

namespace engine {

class Controller;

class Stage {
    ///----------------///
    ///  Member types  ///
    ///----------------///

public:
    using System = std::function<void(Controller&)>;

private:
    class Builder;
    friend BuilderBase<Stage>;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] Stage(const Stage&) = delete;
    [[nodiscard]] Stage(Stage&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto empty() const noexcept -> bool;
    void run(Controller& t_controller) const;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    [[nodiscard]] Stage() noexcept = default;

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::vector<System> m_systems;
};

class Stage::Builder : public BuilderBase<Stage> {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    using BuilderBase<Stage>::BuilderBase;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto add_system(Stage::System&& t_system) -> Builder&;
};

}   // namespace engine
