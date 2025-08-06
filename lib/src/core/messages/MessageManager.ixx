module;

#include <flat_map>
#include <ranges>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module core.messages.MessageManager;

import core.messages.message_c;
import core.messages.MessageBuffer;
import core.messages.ErasedMessageBuffer;

import utility.containers.Any;
import utility.contracts;
import utility.meta.type_traits.const_like;

namespace core::messages {

export class MessageManager {
public:
    explicit MessageManager(
        std::flat_map<std::type_index, ErasedMessageBuffer>&& message_buffers
    );

    template <message_c Message_T, typename Self_T>
    [[nodiscard]]
    auto message_buffer(this Self_T&)
        -> util::meta::const_like_t<MessageBuffer<Message_T>, Self_T>&;

    template <message_c Message_T>
    [[nodiscard]]
    auto manages_message() const noexcept -> bool;

    auto clear_messages() -> void;

private:
    std::flat_map<std::type_index, ErasedMessageBuffer> m_message_buffers;
};

}   // namespace core::messages

template <core::messages::message_c Message_T, typename Self_T>
auto core::messages::MessageManager::message_buffer(this Self_T& self)
    -> util::meta::const_like_t<MessageBuffer<Message_T>, Self_T>&
{
    const auto iter{ self.m_message_buffers.find(typeid(Message_T)) };
    PRECOND(iter != self.m_message_buffers.cend());
    return util::any_cast<MessageBuffer<Message_T>>(iter->second);
}

template <core::messages::message_c Message_T>
auto core::messages::MessageManager::manages_message() const noexcept -> bool
{
    return m_message_buffers.contains(typeid(Message_T));
}

module :private;

core::messages::MessageManager::MessageManager(
    std::flat_map<std::type_index, ErasedMessageBuffer>&& message_buffers
)
    : m_message_buffers{ std::move(message_buffers) }
{}

auto core::messages::MessageManager::clear_messages() -> void
{
    for (auto& message_buffer : m_message_buffers | std::views::values) {
        message_buffer.clear();
    }
}
