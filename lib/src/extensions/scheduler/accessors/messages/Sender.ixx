module;

#include <concepts>
#include <functional>
#include <vector>

export module extensions.scheduler.accessors.messages.Sender;

import core.messages;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_front;
import utility.TypeList;

namespace extensions::scheduler::accessors {

inline namespace messages {

export template <core::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
class Sender {
public:
    using Messages = util::TypeList<Messages_T...>;

    constexpr explicit Sender(
        core::messages::MessageBuffer<Messages_T>&... message_buffers
    );

    template <typename... Args_T>
        requires(sizeof...(Messages_T) == 1)
             && std::constructible_from<
                    util::meta::type_list_front_t<util::TypeList<Messages_T...>>,
                    Args_T&&...>
    constexpr auto send(Args_T&&... args) const -> void;

    template <typename Message_T, typename... Args_T>
        requires(util::meta::type_list_contains_v<util::TypeList<Messages_T...>, Message_T>)
             && std::constructible_from<Message_T, Args_T&&...>
    constexpr auto send(Args_T&&... args) const -> void;

private:
    std::tuple<std::reference_wrapper<core::messages::MessageBuffer<Messages_T>>...>
        m_message_buffer_refs;
};

}   // namespace messages

}   // namespace extensions::scheduler::accessors

template <core::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
constexpr extensions::scheduler::accessors::messages::Sender<Messages_T...>::Sender(
    core::messages::MessageBuffer<Messages_T>&... message_buffers
)
    : m_message_buffer_refs{ message_buffers... }
{}

template <core::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
template <typename... Args_T>
    requires(sizeof...(Messages_T) == 1)
         // TODO: use `Messages_T...[0]` -
         // https://github.com/llvm/llvm-project/issues/138255
         && std::constructible_from<
                util::meta::type_list_front_t<util::TypeList<Messages_T...>>,
                Args_T&&...>
constexpr auto extensions::scheduler::accessors::messages::Sender<Messages_T...>::send(
    Args_T&&... args
) const -> void
{
    std::get<0>(m_message_buffer_refs).get().emplace_back(std::forward<Args_T>(args)...);
}

template <core::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
template <typename Message_T, typename... Args_T>
    requires(util::meta::type_list_contains_v<util::TypeList<Messages_T...>, Message_T>)
         && std::constructible_from<Message_T, Args_T&&...>
constexpr auto extensions::scheduler::accessors::messages::Sender<Messages_T...>::send(
    Args_T&&... args
) const -> void
{
    std::get<util::meta::type_list_index_of_v<util::TypeList<Messages_T...>, Message_T>>(
        m_message_buffer_refs
    )
        .get()
        .emplace_back(std::forward<Args_T>(args)...);
}
