#pragma once

#include <ranges>
#include <vector>

#include <gsl/pointers>

#include "common/patterns/builder/helper.hpp"

class Controller;

class Stage {
    ///----------------///
    ///  Member types  ///
    ///----------------///

public:
    using System = gsl::not_null<void (*)(Controller&)>;

private:
    class Builder;
    friend BuilderBase<Stage>;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] constexpr Stage(const Stage&) = delete;
    [[nodiscard]] constexpr Stage(Stage&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    void run(Controller& controller) const;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    [[nodiscard]] constexpr static auto create() noexcept;
    [[nodiscard]] constexpr static auto empty(const Stage& stage) noexcept;

private:
    [[nodiscard]] constexpr Stage() noexcept = default;

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::vector<System> systems;
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
    [[nodiscard]] constexpr auto add_system(Stage::System&& system);
};

/// ////////////////////// ///
///     IMPLEMENTATION     ///
/// ////////////////////// ///

[[nodiscard]] constexpr auto Stage::create() noexcept {
    return Builder{};
}

[[nodiscard]] constexpr auto Stage::empty(const Stage& stage) noexcept {
    return std::ranges::empty(stage.systems);
}

[[nodiscard]] constexpr auto Stage::Builder::add_system(System&& system) {
    draft().systems.push_back(std::move(system));

    return std::move(*this);
}
