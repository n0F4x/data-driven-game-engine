#pragma once

#include <atomic>
#include <unordered_map>
#include <vector>
#include <utility>
#include <format>
#include <iostream>
#include <ranges>
#include <functional>

#include <gsl/pointers>

#include <entt/entt.hpp>

#include "patterns/builder/helper.hpp"
#include "engine/State.hpp"
#include "engine/Stage.hpp"

class App;


class AppBase {
    friend App;
    friend BuilderBase<AppBase>;

    [[nodiscard]] AppBase() noexcept = default;
    [[nodiscard]] AppBase(const AppBase&) = delete;
    [[nodiscard]] AppBase(AppBase&&) noexcept = default;


    std::string name{ "App" };

    std::unordered_map<State::Id, const State> states;
    gsl::not_null<const State*> nextState = State::invalid_state();
    gsl::not_null<const State*> currentState = State::invalid_state();
    gsl::not_null<const State*> prevState = State::invalid_state();

    std::vector<Stage> stages;
};

class App final {
    class Builder;
    friend Controller;

public:
    [[nodiscard]] App() noexcept = delete;
    [[nodiscard]] App(const App&) = delete;
    [[nodiscard]] App(App&&) noexcept = delete;

    explicit [[nodiscard]] App(AppBase&& base) noexcept :
        name{ std::move(base.name) },
        states{ std::move(base.states) },
        nextState{ base.nextState },
        currentState{ base.currentState },
        prevState{ base.prevState },
        stages{ std::move(base.stages) } {}

    static [[nodiscard]] auto create() noexcept;

    void run();

private:
    void transition() noexcept {
        if (nextState != currentState) {
            currentState->exited();

            prevState = currentState;
            currentState = nextState;

            currentState->entered();
        }
    }

    std::atomic<bool> running = false;

    std::string name{ "App" };

    std::unordered_map<State::Id, const State> states;
    gsl::not_null<const State*> nextState = State::invalid_state();
    gsl::not_null<const State*> currentState = State::invalid_state();
    gsl::not_null<const State*> prevState = State::invalid_state();

    std::vector<Stage> stages;
};


class App::Builder final : public BuilderBase<AppBase> {
public:
    using BuilderBase<AppBase>::BuilderBase;

    explicit(false) [[nodiscard]] operator App() noexcept {
        return build();
    }
    [[nodiscard]] auto build() noexcept -> App {
        return App{ BuilderBase::build() };
    }

    [[nodiscard]] auto add_name(auto new_name) noexcept {
        draft().name = new_name;

        return std::move(*this);
    }

    [[nodiscard]] auto add_state(State&& state) {
        if (State::invalid(*draft().currentState))
            draft().currentState = &draft().states.try_emplace(state.get_id(), std::move(state)).first->second;
        else
            draft().states.try_emplace(state.get_id(), std::move(state));

        return std::move(*this);
    }

    [[nodiscard]] auto add_stage(Stage&& stage) {
        if (!Stage::empty(stage))
            draft().stages.push_back(std::move(stage));

        return std::move(*this);
    }
};

[[nodiscard]] auto App::create() noexcept {
    return Builder{};
}



class Controller final {
public:
    explicit [[nodiscard]] Controller(App& app) noexcept : app{ app } {}
    [[nodiscard]] Controller(const Controller&) = delete;
    [[nodiscard]] Controller(Controller&&) noexcept = delete;

    void quit() noexcept {
        app.running = false;
        app.nextState = State::invalid_state();
    }

    void transition_to(State::Id to) noexcept {
        if (app.nextState == app.currentState)
            if (auto iter{ app.states.find(to) }; iter != app.states.end())
                app.nextState = &iter->second;
    }

    void transition_to_prev() noexcept {
        if (app.nextState == app.currentState)
            app.nextState = app.prevState;
    }

private:
    App& app;
};


void App::run() {
    running = true;
    std::cout << std::format("{} is running...\n", name);

    currentState->entered();

    if (!std::ranges::empty(stages)) {
        Controller controller{ *this };

        while (running) {
            std::ranges::for_each(stages, std::bind_back(&Stage::run, std::ref(controller)));

            transition();
        }
    }

    currentState->exited();
}
