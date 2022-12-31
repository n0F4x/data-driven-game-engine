#pragma once

#include <vector>
#include <ranges>

#include <gsl/pointers>

#include <patterns/builder/helper.hpp>

class Controller;


class Stage {
  ///----------------///
 ///  Member types  ///
///----------------///
public:
    using System = gsl::not_null<void(*)(Controller&)>;

private:
    class Builder;
    friend BuilderBase<Stage>;

public:
  ///------------------------------///
 ///  Constructors / Destructors  ///
///------------------------------///
    constexpr [[nodiscard]] Stage(const Stage&) = delete;
    constexpr [[nodiscard]] Stage(Stage&&) noexcept = default;

  ///--------------------///
 ///  Member functions  ///
///--------------------///
    void run(Controller& controller) const;

  ///------------------///
 ///  Static helpers  ///
///------------------///
    constexpr static [[nodiscard]] auto create() noexcept;
    constexpr static [[nodiscard]] auto empty(const Stage& stage) noexcept;

private:
    constexpr [[nodiscard]] Stage() noexcept = default;

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
    constexpr [[nodiscard]] auto add_system(Stage::System&& system);
};


/// ////////////////////// ///
///     IMPLEMENTATION     ///
/// ////////////////////// ///

constexpr [[nodiscard]] auto Stage::create() noexcept {
    return Builder{};
}

constexpr [[nodiscard]] auto Stage::empty(const Stage& stage) noexcept {
    return std::ranges::empty(stage.systems);
}


constexpr [[nodiscard]] auto Stage::Builder::add_system(System&& system) {
    draft().systems.push_back(std::move(system));

    return std::move(*this);
}
