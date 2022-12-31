#pragma once

#include <atomic>
#include <unordered_map>
#include <vector>
#include <utility>
#include <format>
#include <iostream>
#include <ranges>
#include <functional>
#include <memory>

#include <gsl/pointers>

#include <entt/entt.hpp>

#include "patterns/builder/helper.hpp"
#include "engine/State.hpp"
#include "engine/Stage.hpp"


class App final {
    class Builder;
    friend BuilderBase<App>;

    friend Controller;

public:
    [[nodiscard]] App(const App&) = delete;
    [[nodiscard]] App(App&&) noexcept = default;

    static [[nodiscard]] auto create() noexcept;

    void run();

private:
    [[nodiscard]] App() noexcept = default;

    void transition() noexcept {
        if (nextState != currentState) {
            currentState->exited();

            prevState = currentState;
            currentState = nextState;

            currentState->entered();
        }
    }

    std::unique_ptr<std::atomic<bool>> running = std::make_unique<std::atomic<bool>>(false);

    std::string name{ "App" };

    std::unordered_map<State::Id, const State> states;
    gsl::not_null<const State*> nextState = State::invalid_state();
    gsl::not_null<const State*> currentState = State::invalid_state();
    gsl::not_null<const State*> prevState = State::invalid_state();

    std::vector<Stage> stages;
};


class App::Builder final : public BuilderBase<App> {
public:
    using BuilderBase<App>::BuilderBase;

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
        *app.running = false;
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
    *running = true;
    std::cout << std::format("{} is running...\n", name);

    currentState->entered();

    if (!std::ranges::empty(stages)) {
        Controller controller{ *this };

        while (*running) {
            std::ranges::for_each(stages, std::bind_back(&Stage::run, std::ref(controller)));

            transition();
        }
    }

    currentState->exited();
}
