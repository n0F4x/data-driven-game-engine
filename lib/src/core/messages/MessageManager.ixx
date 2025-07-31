module;

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

export module core.messages.MessageManager;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

import core.messages.message_c;

namespace core::messages {

export template <message_c... Messages_T>
class MessageManager {
public:
    template <message_c Message_T>
    constexpr static std::bool_constant<
        util::meta::type_list_contains_v<util::TypeList<Messages_T...>, Message_T>>
        registered;

    template <message_c Message_T, typename Self_T>
    [[nodiscard]]
    auto message_buffer(this Self_T&)
        -> util::meta::const_like_t<std::vector<Message_T>, Self_T>&   //
        requires(registered<Message_T>());

    auto clear_messages() -> void;

private:
    std::tuple<std::vector<Messages_T>...> m_message_buffers;
};

}   // namespace core::messages

template <core::messages::message_c... Messages_T>
template <core::messages::message_c Message_T, typename Self_T>
auto core::messages::MessageManager<Messages_T...>::message_buffer(this Self_T& self)
    -> util::meta::const_like_t<std::vector<Message_T>, Self_T>&   //
    requires(registered<Message_T>())                              //
{                                                                  //
    return std::get<std::vector<Message_T>>(self.m_message_buffers);
}

template <core::messages::message_c... Messages_T>
auto core::messages::MessageManager<Messages_T...>::clear_messages() -> void
{
    util::meta::for_each<std::make_index_sequence<sizeof...(Messages_T)>>(
        [this]<std::size_t index_T> { std::get<index_T>(m_message_buffers).clear(); }
    );
}
