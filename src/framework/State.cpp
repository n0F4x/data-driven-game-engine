#include "framework/State.hpp"

namespace fw {

const State State::s_invalid_state;

gsl::not_null<const State*> State::invalid_state() noexcept {
    return &s_invalid_state;
}

auto State::invalid(const State& state) noexcept -> bool {
    return state.id == 0;
}

auto State::get_id() const noexcept -> config::Id {
    return id;
}

void State::entered() const noexcept {
    if (onEnter) {
        onEnter();
    }
}

void State::exited() const noexcept {
    if (onExit) {
        onExit();
    }
}

auto State::Builder::on_enter(Action&& callback) noexcept -> Self {
    draft().onEnter = std::move(callback);

    return std::move(*this);
}

auto State::Builder::on_exit(Action&& callback) noexcept -> Self {
    draft().onExit = std::move(callback);

    return std::move(*this);
}

}   // namespace fw
