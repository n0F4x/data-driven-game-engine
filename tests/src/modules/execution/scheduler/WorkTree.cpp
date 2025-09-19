#include <atomic>
#include <expected>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

import ddge.modules.execution.scheduler.Work;
import ddge.modules.execution.scheduler.WorkContinuation;
import ddge.modules.execution.scheduler.WorkTree;

import ddge.utility.contracts;

TEST_CASE("ddge::exec::WorkHub")
{
    ddge::exec::WorkTree work_hub{ 256, 3 };

    SECTION("empty")
    {
        const bool executed = work_hub.try_execute_one_work(0);
        REQUIRE(executed == false);
    }

    SECTION("single work")
    {
        bool                                                   executed{};
        std::expected<ddge::exec::WorkIndex, ddge::exec::Work> work_index{
            work_hub.reserve_slot([&executed] {
                executed = true;
                return ddge::exec::WorkContinuation::eRelease;
            })
        };

        REQUIRE(work_hub.try_execute_one_work(0) == false);

        work_hub.schedule(*work_index);

        REQUIRE(work_hub.try_execute_one_work(0) == true);
        REQUIRE(executed == true);

        REQUIRE(work_hub.try_execute_one_work(0) == false);
    }

    SECTION("reschedule")
    {
        std::expected<ddge::exec::WorkIndex, ddge::exec::Work> work_index{
            work_hub.reserve_slot([rescheduled = false] mutable {
                if (!rescheduled) {
                    rescheduled = true;
                    return ddge::exec::WorkContinuation::eReschedule;
                }
                return ddge::exec::WorkContinuation::eRelease;
            })
        };

        REQUIRE(work_hub.try_execute_one_work(0) == false);

        work_hub.schedule(*work_index);

        REQUIRE(work_hub.try_execute_one_work(0) == true);
        REQUIRE(work_hub.try_execute_one_work(0) == true);

        REQUIRE(work_hub.try_execute_one_work(0) == false);
    }

    SECTION("empty work is not allowed")
    {
        REQUIRE_THROWS_AS(
            work_hub.reserve_slot(nullptr), ddge::util::PreconditionViolation
        );
    }

    SECTION("release")
    {
        bool released{};
        std::expected<
            ddge::exec::WorkIndex,
            std::pair<ddge::exec::Work, ddge::exec::ReleaseWorkContract>>
            work_index{ work_hub.reserve_slot(
                [] { return ddge::exec::WorkContinuation::eRelease; },
                [&released] { released = true; }
            ) };

        REQUIRE(work_hub.try_execute_one_work(0) == false);

        work_hub.schedule(*work_index);

        REQUIRE(work_hub.try_execute_one_work(0) == true);
        REQUIRE(released == false);

        REQUIRE(work_hub.try_execute_one_work(0) == true);
        REQUIRE(released == true);

        REQUIRE(work_hub.try_execute_one_work(0) == false);
    }

    SECTION("empty release is not called")
    {
        std::expected<
            ddge::exec::WorkIndex,
            std::pair<ddge::exec::Work, ddge::exec::ReleaseWorkContract>>
            work_index{ work_hub.reserve_slot(
                [] { return ddge::exec::WorkContinuation::eRelease; },   //
                nullptr
            ) };

        REQUIRE(work_hub.try_execute_one_work(0) == false);

        work_hub.schedule(*work_index);

        REQUIRE(work_hub.try_execute_one_work(0) == true);
        REQUIRE(work_hub.try_execute_one_work(0) == false);
    }

    SECTION("simple multithreading")
    {
        constexpr static uint32_t work_count{ 256 };
        constexpr static uint32_t per_work_count{ 4 };
        constexpr static uint32_t goal{ work_count * per_work_count };
        constexpr static uint32_t thread_count{ 8 };
        std::atomic_uint32_t      counter;

        const auto process_work = [&work_hub, &counter](const uint32_t thread_id) {
            return [&work_hub, &counter, thread_id] {
                while (counter < goal) {
                    work_hub.try_execute_one_work(
                        thread_id % work_hub.optimized_for_thread_count()
                    );
                }
            };
        };

        for (const auto _ : std::views::repeat(std::ignore, work_count)) {
            const ddge::exec::WorkIndex work_index =
                *work_hub.reserve_slot([&counter, local_counter = 0u] mutable {
                    if (local_counter++ < per_work_count) {
                        ++counter;
                        return ddge::exec::WorkContinuation::eReschedule;
                    }
                    return ddge::exec::WorkContinuation::eRelease;
                });

            work_hub.schedule(work_index);
        }

        std::vector<std::jthread> threads;
        threads.reserve(thread_count);
        for (const uint32_t thread_id : std::views::iota(0u, thread_count)) {
            threads.emplace_back(process_work(thread_id));
        }

        for (std::jthread& thread : threads) {
            thread.join();
        }

        REQUIRE(counter == goal);
    }
}
