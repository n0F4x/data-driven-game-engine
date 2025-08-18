#include <algorithm>
#include <atomic>
#include <chrono>
#include <optional>
#include <print>
#include <ranges>
#include <thread>

#include <catch2/catch_test_macros.hpp>

import ddge.modules.execution.scheduler.SignalTree;

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

                    for (const auto index : std::views::iota(0u, signal_tree.capacity())) {
                        signal_tree.set(index);
                    }

                    std::vector<bool> index_checks(signal_tree.capacity());
                    for (auto _ : std::views::repeat(std::ignore, signal_tree.capacity())) {
                        const std::optional<ddge::exec::SignalIndex> signal_index{
                            signal_tree.try_unset_one(default_strategy)
                        };

                        REQUIRE(signal_index.has_value());
                        REQUIRE(signal_index.value() < signal_tree.capacity());

                        index_checks.at(signal_index.value()) = true;
                    }

                    const bool all_checked{
                        std::ranges::all_of(index_checks, std::identity{})
                    };
                    REQUIRE(all_checked);
                }
            };

            test(3);
            test(5);
            test(7);
            test(12);
        }

        SECTION("multi-threaded")
        {
            constexpr static auto test = [](uint32_t number_of_levels,
                                            uint32_t number_of_threads) -> void {
                SECTION(std::format({ "{} levels" }, number_of_levels))
                {
                    ddge::exec::SignalTree signal_tree{ number_of_levels };

                    std::vector<std::atomic_int> index_checks(signal_tree.capacity());

                    const auto work = [&signal_tree,
                                       &index_checks,
                                       number_of_threads](const uint32_t id) {
                        return [&signal_tree, &index_checks, id, number_of_threads] {
                            uint32_t number_of_signals{};

                            for (const auto index :
                                 std::views::iota(0u, signal_tree.capacity()))
                            {
                                if (index % number_of_threads == id) {
                                    signal_tree.set(index);
                                    ++number_of_signals;
                                }
                            }

                            for (auto _ :
                                 std::views::repeat(std::ignore, number_of_signals))
                            {
                                const std::optional<ddge::exec::SignalIndex> signal_index{
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

            test(3, std::jthread::hardware_concurrency());
            test(7, std::jthread::hardware_concurrency());
            test(12, std::jthread::hardware_concurrency());
            test(16, std::jthread::hardware_concurrency());
        }
    }
}
