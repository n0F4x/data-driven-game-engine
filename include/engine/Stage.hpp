#pragma once

#include <ranges>
#include <vector>

#include <gsl/pointers>

#include "common/patterns/builder/helper.hpp"

namespace engine {

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
    [[nodiscard]] constexpr auto empty() noexcept;
    void run(Controller& t_controller) const;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    [[nodiscard]] constexpr static auto create() noexcept;

private:
    [[nodiscard]] constexpr Stage() noexcept = default;

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
    [[nodiscard]] constexpr auto add_system(Stage::System&& t_system)
        -> Builder&;
};

/// ////////////////////// ///
///     IMPLEMENTATION     ///
/// ////////////////////// ///

[[nodiscard]] constexpr auto Stage::empty() noexcept {
    return std::ranges::empty(m_systems);
}

[[nodiscard]] constexpr auto Stage::create() noexcept {
    return Builder{};
}

[[nodiscard]] constexpr auto Stage::Builder::add_system(System&& t_system)
    -> Builder& {
    draft().m_systems.push_back(std::move(t_system));

    return *this;
}

}   // namespace engine
