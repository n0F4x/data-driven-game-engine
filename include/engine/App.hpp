#pragma once

#include <atomic>
#include <unordered_map>
#include <vector>
#include <memory>

#include <gsl/pointers>

#include "config/id.hpp"
#include "patterns/builder/helper.hpp"
#include "engine/State.hpp"

class Controller;
class Stage;


class App final {
  ///----------------///
 ///  Member types  ///
///----------------///
    class Builder;
    friend BuilderBase<App>;

public:
    friend Controller;

  ///------------------------------///
 ///  Constructors / Destructors  ///
///------------------------------///
    [[nodiscard]] App(const App&) = delete;
    [[nodiscard]] App(App&&) noexcept = default;

  ///--------------------///
 ///  Member functions  ///
///--------------------///
    void run();

  ///------------------///
 ///  Static helpers  ///
///------------------///
    static [[nodiscard]] auto create() noexcept -> Builder;

private:
    [[nodiscard]] App() noexcept = default;

    void transition() noexcept;

  ///--------------------///
 ///  Member variables  ///
///--------------------///
    std::unique_ptr<std::atomic<bool>> running = std::make_unique<std::atomic<bool>>(false);

    std::string name = "App";

    std::unordered_map<Id, const State> states;
    gsl::not_null<const State*> nextState = State::invalid_state();
    gsl::not_null<const State*> currentState = State::invalid_state();
    gsl::not_null<const State*> prevState = State::invalid_state();

    std::vector<Stage> stages;
};


class App::Builder final : public BuilderBase<App> {
public:
  ///------------------------------///
 ///  Constructors / Destructors  ///
///------------------------------///
    using BuilderBase<App>::BuilderBase;

  ///--------------------///
 ///  Member functions  ///
///--------------------///
    [[nodiscard]] auto set_name(std::string_view new_name) noexcept -> Self;
    [[nodiscard]] auto add_state(State&& state) -> Self;
    [[nodiscard]] auto add_stage(Stage&& stage) -> Self;
};

