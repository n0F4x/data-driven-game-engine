#pragma once

#include <vector>
#include <future>

#include <gsl/pointers>

#include <patterns/builder/helper.hpp>

class Controller;


class Stage {
public:
    using System = gsl::not_null<void(*)(Controller&)>;

private:
    class Builder;
    friend BuilderBase<Stage>;

public:
    constexpr [[nodiscard]] Stage(const Stage&) = delete;
    constexpr [[nodiscard]] Stage(Stage&&) noexcept = default;

    constexpr static [[nodiscard]] auto create() noexcept;

    void run(Controller& controller) const {
        std::vector<std::future<void>> futures;
        futures.resize(systems.size());

        for (auto& system : systems)
            futures.push_back(std::async(std::launch::async, system, std::ref(controller)));
    }

    constexpr static [[nodiscard]] auto empty(const Stage& stage) noexcept {
        return std::ranges::empty(stage.systems);
    }

private:
    constexpr [[nodiscard]] Stage() noexcept = default;

    std::vector<System> systems;
};


class Stage::Builder : public BuilderBase<Stage> {
public:
    using BuilderBase<Stage>::BuilderBase;

    constexpr [[nodiscard]] auto add_system(System&& system) {
        draft().systems.push_back(std::move(system));

        return std::move(*this);
    }
};

constexpr [[nodiscard]] auto Stage::create() noexcept {
    return Builder{};
}
