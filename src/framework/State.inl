#include <utility>

namespace fw {

template <typename TId>
BasicState<TId>::BasicState(const Id t_id,
                            Action&& t_enterAction,
                            Action&& t_exitAction) noexcept
    : m_id{ t_id },
      m_enterAction{ std::move(t_enterAction) },
      m_exitAction{ std::move(t_exitAction) } {}

template <typename TId>
auto BasicState<TId>::id() const noexcept -> Id {
    return m_id;
}

template <typename TId>
void BasicState<TId>::enter() const noexcept {
    if (m_enterAction) {
        m_enterAction();
    }
}

template <typename TId>
void BasicState<TId>::exit() const noexcept {
    if (m_exitAction) {
        m_exitAction();
    }
}

template <typename TId>
BasicState<TId>::Builder::operator Product() noexcept {
    return build();
}

template <typename TId>
auto BasicState<TId>::Builder::build() noexcept -> Product {
    return Product{ m_id, std::move(m_enterAction), std::move(m_exitAction) };
}

template <typename TId>
auto BasicState<TId>::Builder::set_id(Id t_id) noexcept -> Builder& {
    m_id = t_id;
    return *this;
}

template <typename TId>
auto BasicState<TId>::Builder::on_enter(Action&& t_callback) noexcept
    -> Builder& {
    m_enterAction = std::move(t_callback);
    return *this;
}

template <typename TId>
auto BasicState<TId>::Builder::on_exit(Action&& t_callback) noexcept
    -> Builder& {
    m_exitAction = std::move(t_callback);
    return *this;
}

}
