export module addons.Messages;

import core.messages;

import extensions.scheduler.argument_provider_for;
import extensions.scheduler.argument_providers.MessageProvider;

namespace addons {

export struct MessagesTag {};

export template <core::messages::message_c... Messages_T>
struct Messages : MessagesTag {
    core::messages::MessageManager<Messages_T...> message_manager;
};

}   // namespace addons

template <core::messages::message_c... Messages_T>
struct extensions::scheduler::ArgumentProviderFor<addons::Messages<Messages_T...>> {
    using type = extensions::scheduler::argument_providers::
        MessageProvider<core::messages::MessageManager<Messages_T...>, addons::Messages<Messages_T...>>;
};
