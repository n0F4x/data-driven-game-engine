#include <chrono>
#include <print>
#include <thread>

import ddge.prelude;
import ddge.modules.scheduler;

using namespace ddge::scheduler;

auto say_hello_from_thread() -> void
{
    std::println("Hello from thread #{}", std::this_thread::get_id());
}

struct ContendedResource {};

auto contend_for_resource_first(accessors::Resource<ContendedResource>) -> void
{
    using namespace std::chrono_literals;
    std::println(
        "first access to contended resource (Thread id #{})", std::this_thread::get_id()
    );
    std::this_thread::sleep_for(3s);
}

auto contend_for_resource_second(accessors::Resource<ContendedResource>) -> void
{
    using namespace std::chrono_literals;
    std::println(
        "second access to contended resource (Thread id #{})", std::this_thread::get_id()
    );
    std::this_thread::sleep_for(3s);
}

auto print_join_message() -> void
{
    std::println(
        "-- Threads joined (current thread id is #{})", std::this_thread::get_id()
    );
}

[[nodiscard]]
constexpr auto always_4() noexcept -> int
{
    return 4;
}

auto main() -> int
{
    using namespace ddge;

    app::create()
        .plug_in(resources::Plugin{})
        .insert_resource(ContendedResource{})
        .plug_in(scheduler::Plugin{ 4 })
        .run(
            scheduler::repeat(
                scheduler::start_as(
                    scheduler::group(
                        scheduler::as_task(say_hello_from_thread),
                        scheduler::as_task(contend_for_resource_first),
                        scheduler::as_task(say_hello_from_thread),
                        scheduler::as_task(say_hello_from_thread),
                        scheduler::as_task(contend_for_resource_second),
                        scheduler::as_task(say_hello_from_thread)
                    )
                )
                    .then(scheduler::as_task(print_join_message)),
                scheduler::as_task(always_4)
            )
        );
}
