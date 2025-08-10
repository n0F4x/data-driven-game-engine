module;

#include <concepts>
#include <functional>
#include <vector>

export module ddge.modules.execution.accessors.messages.Sender;

import ddge.modules.messages;

import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_index_of;
import ddge.utility.meta.type_traits.type_list.type_list_front;
import ddge.utility.TypeList;

namespace ddge::exec::accessors {

inline namespace messages {

export template <ddge::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
class Sender {
public:
    using Messages = util::TypeList<Messages_T...>;

    constexpr explicit Sender(
        ddge::messages::MessageBuffer<Messages_T>&... message_buffers
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
    std::tuple<std::reference_wrapper<ddge::messages::MessageBuffer<Messages_T>>...>
        m_message_buffer_refs;
};

}   // namespace messages

}   // namespace ddge::exec::accessors

template <ddge::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
constexpr ddge::exec::accessors::messages::Sender<Messages_T...>::Sender(
    ddge::messages::MessageBuffer<Messages_T>&... message_buffers
)
    : m_message_buffer_refs{ message_buffers... }
{}

template <ddge::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
template <typename... Args_T>
    requires(sizeof...(Messages_T) == 1)
         // TODO: use `Messages_T...[0]` -
         // https://github.com/llvm/llvm-project/issues/138255
         && std::constructible_from<
                ddge::util::meta::type_list_front_t<ddge::util::TypeList<Messages_T...>>,
                Args_T&&...>
constexpr auto ddge::exec::accessors::messages::Sender<Messages_T...>::send(
    Args_T&&... args
) const -> void
{
    std::get<0>(m_message_buffer_refs).get().emplace_back(std::forward<Args_T>(args)...);
}

template <ddge::messages::message_c... Messages_T>
    requires(sizeof...(Messages_T) != 0)
template <typename Message_T, typename... Args_T>
    requires(ddge::util::meta::
                 type_list_contains_v<ddge::util::TypeList<Messages_T...>, Message_T>)
         && std::constructible_from<Message_T, Args_T&&...>
constexpr auto ddge::exec::accessors::messages::Sender<Messages_T...>::send(
    Args_T&&... args
) const -> void
{
    std::get<util::meta::type_list_index_of_v<util::TypeList<Messages_T...>, Message_T>>(
        m_message_buffer_refs
    )
        .get()
        .emplace_back(std::forward<Args_T>(args)...);
}
