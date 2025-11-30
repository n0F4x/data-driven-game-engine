#include <chrono>
#include <print>
#include <thread>

import ddge.prelude;
import ddge.modules.exec;

using namespace ddge::exec::accessors;

auto say_hello_from_thread() -> void
{
    std::println("Hello from thread #{}", std::this_thread::get_id());
}

struct ContendedResource {};

auto contend_for_resource_first(Resource<ContendedResource>) -> void
{
    using namespace std::chrono_literals;
    std::println(
        "first access to contended resource (Thread id #{})", std::this_thread::get_id()
    );
    std::this_thread::sleep_for(3s);
}

auto contend_for_resource_second(Resource<ContendedResource>) -> void
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
        .plug_in(plugins::Resources{})
        .insert_resource(ContendedResource{})
        .plug_in(plugins::Execution{ 4 })
        .run(
            exec::repeat(
                exec::start_as(
                    exec::group(
                        exec::as_task(say_hello_from_thread),         //
                        exec::as_task(contend_for_resource_first),    //
                        exec::as_task(say_hello_from_thread),         //
                        exec::as_task(say_hello_from_thread),         //
                        exec::as_task(contend_for_resource_second),   //
                        exec::as_task(say_hello_from_thread)          //
                    )
                )
                    .then(exec::as_task(print_join_message)),
                exec::as_task(always_4)   //
            )                             //
        );
}
