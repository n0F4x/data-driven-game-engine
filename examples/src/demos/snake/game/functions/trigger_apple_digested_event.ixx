export module snake.game.trigger_apple_digested_event;

import extensions.scheduler.accessors.events.Recorder;

import snake.game.AppleDigested;

using namespace extensions::scheduler::accessors;

namespace game {

export constexpr inline auto trigger_apple_digested_event =
    [](const ::events::Recorder<AppleDigested>& recorder)   //
{                                                           //
    recorder.record();
};

}   // namespace game
