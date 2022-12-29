#pragma once

#include <atomic>
#include <unordered_map>
#include <vector>
#include <utility>
#include <format>
#include <iostream>
#include <ranges>

#include <gsl/pointers>

#include <entt/entt.hpp>

#include "patterns/builder/helper.hpp"
#include "engine/State.hpp"
#include "engine/Stage.hpp"

class App;


class AppBase {
    friend App;
    class Builder;
    friend BuilderBase<AppBase>;

public:
    [[nodiscard]] static auto create() noexcept;

private:
    [[nodiscard]] AppBase() noexcept = default;
    [[nodiscard]] AppBase(AppBase&&) noexcept = default;


protected:
    std::string name{ "App" };

    std::unordered_map<State::Id, State> states;
    gsl::not_null<State*> nextState = &State::invalid_state();
    gsl::not_null<State*> currentState = &State::invalid_state();
    gsl::not_null<State*> prevState = &State::invalid_state();

    std::vector<Stage> stages;
};


class AppBase::Builder final : public BuilderBase<AppBase> {
public:
    using BuilderBase<AppBase>::BuilderBase;

    [[nodiscard]] explicit(false) operator App() noexcept;
    [[nodiscard]] App build() noexcept;

    [[nodiscard]] auto add_name(auto new_name) noexcept {
        draft().name = new_name;

        return std::move(*this);
    }

    [[nodiscard]] auto add_state(State&& state) {
        if (!is_valid(*draft().currentState))
            draft().currentState = &draft().states.try_emplace(state.get_id(), std::move(state)).first->second;
        else
            draft().states.try_emplace(state.get_id(), std::move(state));

        return std::move(*this);
    }

    [[nodiscard]] auto add_stage(Stage&& stage) {
        draft().stages.push_back(std::move(stage));

        return std::move(*this);
    }
};

auto AppBase::create() noexcept {
    return Builder{};
}


class App final : public AppBase {
    friend App::Builder;
    friend Controller;

public:
    void run();

private:
    [[nodiscard]] explicit App(AppBase&& base) noexcept : AppBase{ std::move(base) } {}

    void transition() noexcept {
        if (nextState != currentState) {
            currentState->exited();

            prevState = currentState;
            currentState = nextState;

            currentState->entered();
        }
    }

    std::atomic<bool> running = false;
};

AppBase::Builder::operator App() noexcept {
    return build();
}

App AppBase::Builder::build() noexcept {
    return App{ BuilderBase::build() };
}


class Controller final {
public:
    [[nodiscard]] explicit Controller(App& app) noexcept : app{ app } {}

    auto quit() noexcept {
        app.running = false;
        app.nextState = &State::invalid_state();
    }

    auto transition_to(State::Id to) noexcept {
        if (app.nextState == app.currentState)
            if (auto iter{ app.states.find(to) }; iter != app.states.end())
                app.nextState = &iter->second;
    }

    auto transition_to_prev() noexcept {
        if (app.nextState == app.currentState)
            app.nextState = app.prevState;
    }

private:
    App& app;
};

void App::run() {
    running = true;
    std::cout << std::format("{} is running...\n", name);

    if (std::ranges::any_of(stages, [](auto& stage) { return stage.has_system(); })) {
        Controller controller{ *this };

        currentState->entered();

        while (running) {
            std::ranges::for_each(stages, [&controller](auto& stage) { stage.run(controller); });

            transition();
        }
    }
}
