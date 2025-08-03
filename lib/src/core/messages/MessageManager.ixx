module;

#include <functional>
#include <utility>

export module core.messages.MessageManager;

import core.messages.message_c;
import core.messages.MessageBuffer;
import core.store.Store;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.tuple.tuple_for_each;
import utility.TypeList;

namespace core::messages {

export class MessageManager {
public:
    template <message_c... Messages_T>
    explicit MessageManager(util::TypeList<Messages_T...>);

    template <message_c Message_T, typename Self_T>
    [[nodiscard]]
    auto message_buffer(this Self_T&)
        -> util::meta::const_like_t<MessageBuffer<Message_T>, Self_T>&;

    template <message_c Message_T>
    [[nodiscard]]
    auto manages_message() const noexcept -> bool;

    auto clear_messages() -> void;

private:
    store::Store          m_message_buffers;
    std::function<void()> m_clear_messages;
};

}   // namespace core::messages

template <core::messages::message_c... Messages_T>
core::messages::MessageManager::MessageManager(util::TypeList<Messages_T...>)
{
    (m_message_buffers.emplace<MessageBuffer<Messages_T>>(), ...);
    m_clear_messages =
        [buffer_refs_tuple =
             std::tuple<std::reference_wrapper<MessageBuffer<Messages_T>>...>{
                 m_message_buffers.at<MessageBuffer<Messages_T>>()... }] {
            // TODO: remove this ignore with next Clang version
            std::ignore = buffer_refs_tuple;
            (std::get<std::reference_wrapper<MessageBuffer<Messages_T>>>(buffer_refs_tuple)
                 .get()
                 .clear(),
             ...);
        };
}

template <core::messages::message_c Message_T, typename Self_T>
auto core::messages::MessageManager::message_buffer(this Self_T& self)
    -> util::meta::const_like_t<MessageBuffer<Message_T>, Self_T>&
{
    return self.m_message_buffers.template at<MessageBuffer<Message_T>>();
}

template <core::messages::message_c Message_T>
auto core::messages::MessageManager::manages_message() const noexcept -> bool
{
    return m_message_buffers.contains<MessageBuffer<Message_T>>();
}

auto core::messages::MessageManager::clear_messages() -> void
{
    m_clear_messages();
}
