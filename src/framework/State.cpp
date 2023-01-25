#include "framework/State.hpp"

namespace fw {

const State State::s_invalidState;

gsl::not_null<const State*> State::invalid_state() noexcept {
    return &s_invalidState;
}

auto State::id() const noexcept -> config::Id {
    return m_id;
}

auto State::invalid() const noexcept -> bool {
    return m_id == 0;
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

auto State::Builder::on_enter(Action&& t_callback) noexcept -> Builder& {
    draft().m_enterAction = std::move(t_callback);

    return *this;
}

auto State::Builder::on_exit(Action&& t_callback) noexcept -> Builder& {
    draft().m_exitAction = std::move(t_callback);

    return *this;
}

}   // namespace fw
