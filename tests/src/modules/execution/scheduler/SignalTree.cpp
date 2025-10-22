#include <algorithm>
#include <atomic>
#include <chrono>
#include <format>
#include <optional>
#include <ranges>
#include <thread>

#include <catch2/catch_test_macros.hpp>

import ddge.modules.exec.scheduler.SignalTree;

auto default_strategy(uint32_t) -> ddge::exec::TravelsalBias
{
    return ddge::exec::TravelsalBias::eLeft;
}

TEST_CASE("ddge::exec::SignalTree")
{
    SECTION("set all signals")
    {
        SECTION("single-threaded")
        {
            constexpr static auto test = [](uint32_t number_of_levels) -> void {
                SECTION(std::format({ "{} levels" }, number_of_levels))
                {
                    ddge::exec::SignalTree signal_tree{ number_of_levels };

                    for (const auto index :
                         std::views::iota(0u, signal_tree.number_of_leaves()))
                    {
                        signal_tree.try_set_one(index);
                    }

                    std::vector<bool> index_checks(signal_tree.number_of_leaves());
                    for (auto _ :
                         std::views::repeat(std::ignore, signal_tree.number_of_leaves()))
                    {
                        const std::optional<ddge::exec::SignalTree::LeafIndex>
                            signal_index{ signal_tree.try_unset_one(default_strategy) };

                        REQUIRE(signal_index.has_value());
                        REQUIRE(signal_index.value() < signal_tree.number_of_leaves());

                        index_checks.at(signal_index.value()) = true;
                    }

                    const bool all_checked{
                        std::ranges::all_of(index_checks, std::identity{})
                    };
                    REQUIRE(all_checked);
                }
            };

            test(2);
            test(3);
            test(5);
            test(7);
            test(12);
        }

        SECTION("multi-threaded")
        {
            constexpr static auto test = [](const uint32_t number_of_levels,
                                            const uint32_t number_of_threads) -> void {
                SECTION(std::format("{} levels", number_of_levels))
                {
                    ddge::exec::SignalTree signal_tree{ number_of_levels };

                    std::vector<std::atomic_int> index_checks(
                        signal_tree.number_of_leaves()
                    );

                    const auto work = [&signal_tree,
                                       &index_checks,
                                       number_of_threads](const uint32_t id) {
                        return [&signal_tree, &index_checks, id, number_of_threads] {
                            uint32_t number_of_signals{};

                            for (const auto index :
                                 std::views::iota(0u, signal_tree.number_of_leaves()))
                            {
                                if (index % number_of_threads == id) {
                                    signal_tree.try_set_one(index);
                                    ++number_of_signals;
                                }
                            }

                            for (auto _ :
                                 std::views::repeat(std::ignore, number_of_signals))
                            {
                                const std::optional<ddge::exec::SignalTree::LeafIndex>
                                    signal_index{
                                        signal_tree.try_unset_one(default_strategy)
                                    };

                                ++index_checks.at(signal_index.value());
                            }
                        };
                    };

                    std::vector<std::jthread> threads;
                    threads.reserve(number_of_threads);
                    for (const auto id : std::views::iota(0u, number_of_threads)) {
                        threads.emplace_back(work(id));
                    }

                    for (auto& thread : threads) {
                        thread.join();
                    }

                    const bool all_checked{ std::ranges::all_of(
                        index_checks,
                        [](const std::atomic_int& counter) { return counter == 1; }
                    ) };

                    REQUIRE(all_checked);
                }
            };

            test(2, std::jthread::hardware_concurrency());
            test(3, std::jthread::hardware_concurrency());
            test(5, std::jthread::hardware_concurrency());
            test(7, std::jthread::hardware_concurrency());
            test(12, std::jthread::hardware_concurrency());
        }
    }

    SECTION("explicitly unset all signals")
    {
        SECTION("single-threaded")
        {
            constexpr static auto test = [](uint32_t number_of_levels) -> void {
                SECTION(std::format({ "{} levels" }, number_of_levels))
                {
                    ddge::exec::SignalTree signal_tree{ number_of_levels };

                    for (const auto index :
                         std::views::iota(0u, signal_tree.number_of_leaves()))
                    {
                        signal_tree.try_set_one(index);
                    }

                    for (const auto i :
                         std::views::iota(0u, signal_tree.number_of_leaves()))
                    {
                        const bool success{ signal_tree.try_unset_one_at(i) };
                        REQUIRE(success);
                    }
                }
            };

            test(2);
            test(3);
            test(5);
            test(7);
            test(12);
        }

        SECTION("multi-threaded")
        {
            constexpr static auto test = [](const uint32_t number_of_levels,
                                            const uint32_t number_of_threads) -> void {
                SECTION(std::format("{} levels", number_of_levels))
                {
                    ddge::exec::SignalTree signal_tree{ number_of_levels };

                    for (const auto index :
                         std::views::iota(0u, signal_tree.number_of_leaves()))
                    {
                        signal_tree.try_set_one(index);
                    }

                    std::vector<bool> index_checks(signal_tree.number_of_leaves());

                    const auto work = [number_of_threads,
                                       &signal_tree,
                                       &index_checks](const uint32_t id) {
                        return [number_of_threads, &signal_tree, &index_checks, id] {
                            for (const auto i : std::views::iota(
                                     0u, signal_tree.number_of_leaves() / number_of_threads
                                 ))
                            {
                                const auto index{ i * number_of_threads + id };
                                const bool success{ signal_tree.try_unset_one_at(index) };
                                index_checks.at(index) = success;
                            }
                            if (signal_tree.number_of_leaves() % number_of_threads > id) {
                                const auto index{ signal_tree.number_of_leaves()
                                                      / number_of_threads
                                                      * number_of_threads
                                                  + id };
                                const bool success{ signal_tree.try_unset_one_at(index) };
                                index_checks.at(index) = success;
                            }
                        };
                    };

                    std::vector<std::jthread> threads;
                    threads.reserve(number_of_threads);
                    for (const auto id : std::views::iota(0u, number_of_threads)) {
                        threads.emplace_back(work(id));
                    }

                    for (auto& thread : threads) {
                        thread.join();
                    }

                    REQUIRE(std::ranges::all_of(index_checks, std::identity{}));
                }
            };

            test(2, std::jthread::hardware_concurrency());
            test(3, std::jthread::hardware_concurrency());
            test(5, std::jthread::hardware_concurrency());
            test(7, std::jthread::hardware_concurrency());
            test(12, std::jthread::hardware_concurrency());
        }
    }

    SECTION("concurrent set and unset")
    {
        constexpr static auto test = [](const uint32_t number_of_threads) -> void {
            SECTION(std::format("{} threads", number_of_threads))
            {
                constexpr static uint32_t work_per_thread{ 64 };
                const uint32_t            goal{ number_of_threads * work_per_thread };
                std::atomic_uint32_t      counter;

                ddge::exec::SignalTree signal_tree{ [number_of_threads] {
                    uint32_t           number_of_levels{ 3 };
                    while ((0x1u << (number_of_levels - 1)) < number_of_threads) {
                        ++number_of_levels;
                    }
                    return number_of_levels;
                }() };

                const auto set_work = [&signal_tree](const uint32_t id) {
                    return [&signal_tree, id] {
                        for (const auto _ :
                             std::views::repeat(std::ignore, work_per_thread))
                        {
                            while (!signal_tree.try_set_one(id))
                                ;
                        }
                    };
                };

                const auto work = [&signal_tree, &counter, goal] {
                    while (counter < goal) {
                        if (signal_tree.try_unset_one(default_strategy).has_value()) {
                            ++counter;
                        }
                    }
                };

                std::vector<std::jthread> consumer_threads;
                consumer_threads.reserve(number_of_threads);
                for (const auto _ : std::views::repeat(std::ignore, number_of_threads)) {
                    consumer_threads.emplace_back(work);
                }

                std::vector<std::jthread> producer_threads;
                producer_threads.reserve(number_of_threads);
                for (const auto id : std::views::iota(0u, number_of_threads)) {
                    producer_threads.emplace_back(set_work(id));
                }

                for (auto& thread : consumer_threads) {
                    thread.join();
                }

                REQUIRE(counter == goal);
            }
        };

        test(2);
        test(3);
        test(5);
        test(7);
        test(12);
    }
}
