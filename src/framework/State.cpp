#include "framework/State.hpp"

namespace fw {

const State State::s_invalidState;

gsl::not_null<const State*> State::invalid_state() noexcept {
    return &s_invalidState;
}

auto State::invalid(const State& t_state) noexcept -> bool {
    return t_state.m_id == 0;
}

auto State::id() const noexcept -> config::Id {
    return m_id;
}

void State::entered() const noexcept {
    if (m_enterAction) {
        m_enterAction();
    }
}

void State::exited() const noexcept {
    if (m_exitAction) {
        m_exitAction();
    }
}

State::State(config::Id t_id) noexcept : m_id{ t_id } {}

auto State::Builder::on_enter(Action&& t_callback) noexcept -> Self {
    draft().m_enterAction = std::move(t_callback);

    return std::move(*this);
}

auto State::Builder::on_exit(Action&& t_callback) noexcept -> Self {
    draft().m_exitAction = std::move(t_callback);

    return std::move(*this);
}

}   // namespace fw
