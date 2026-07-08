module;

#include <atomic>
#include <cstdint>
#include <memory>
#include <ranges>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "ddge/util/contract_macros.hpp"
#include "ddge/util/no_unique_address.hpp"

export module ddge.scheduler.Plugin;

import ddge.app.add_on_t;
import ddge.app.builder_c;
import ddge.app.decays_to_app_c;
import ddge.scheduler.accessors;
import ddge.scheduler.Nexus;
import ddge.scheduler.data_structures.WorkTree;
import ddge.scheduler.provider_c;
import ddge.scheduler.ProviderOf;
import ddge.scheduler.TaskBuilder;
import ddge.scheduler.TaskContinuation;
import ddge.scheduler.TaskContinuationFactory;
import ddge.scheduler.TaskHub;
import ddge.scheduler.TaskHubBuilder;
import ddge.scheduler.TaskHubProxy;
import ddge.scheduler.TypedTaskFactoryHandle;

import ddge.util.contracts;
import ddge.util.meta.type_traits.type_list.type_list_filter;
import ddge.util.meta.type_traits.type_list.type_list_transform;
import ddge.util.TypeList;

namespace ddge::scheduler {

export class Plugin {
public:
    explicit Plugin(uint32_t number_of_threads = std::jthread::hardware_concurrency());

    template <app::builder_c Self_T>
    auto run(this Self_T&&, TaskBuilder<void>&& task_builder) -> void;

private:
    [[engine_no_unique_address]] struct Precondition {
        explicit Precondition(uint32_t number_of_threads);
    } m_precondition;

    // TODO: remove this maybe_unused - it is actually used inside `run`
    [[maybe_unused]]
    uint32_t m_number_of_threads;
};

}   // namespace ddge::scheduler

template <typename App_T>
struct AddonTraits {
    template <typename Addon_T>
        struct HasAccessorProvider : std::bool_constant < requires {
        ddge::scheduler::provider_c<ddge::scheduler::provider_for_t<Addon_T>, App_T>;
    }>{};

    template <typename Addon_T>
    struct AccessorProvider {
        using type = ddge::scheduler::provider_for_t<Addon_T>;
    };
};

ddge::scheduler::Plugin::Precondition::Precondition(
    [[maybe_unused]] const uint32_t number_of_threads
)
{
    PRECOND(number_of_threads > 0, "Number of threads must be greater than zero!");
}

ddge::scheduler::Plugin::Plugin(const uint32_t number_of_threads)
    : m_precondition{ number_of_threads },
      m_number_of_threads{ number_of_threads }
{}

template <ddge::app::builder_c Self_T>
auto ddge::scheduler::Plugin::run(this Self_T&& self, TaskBuilder<void>&& task_builder)
    -> void
{
    const uint32_t number_of_threads{ self.ddge::scheduler::Plugin::m_number_of_threads };

    auto app{ std::forward<Self_T>(self).build() };
    using App                       = decltype(app);
    using Addons                    = App::Addons;
    using AccessorProvidersTypeList = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<Addons, AddonTraits<App>::template HasAccessorProvider>,
        AddonTraits<App>::template AccessorProvider>;

    [number_of_threads, &task_builder, &app]<typename... AccessorProviders_T>   //
        (util::TypeList<AccessorProviders_T...>)                                //
    {
        Nexus          nexus{ AccessorProviders_T{ app }... };
        TaskHubBuilder task_hub_builder{ nexus };

        std::atomic_bool                   should_stop{};
        const TypedTaskFactoryHandle<void> root_task_handle =
            std::move(task_builder).build(task_hub_builder);
        root_task_handle->set_continuation_factory(
            TaskContinuationFactory<void>{
                [&should_stop](const TaskHubProxy) -> TaskContinuation<void> {
                    return TaskContinuation<void>{
                        [&should_stop] -> void { should_stop = true; }   //
                    };
                }   //
            }
        );

        const std::unique_ptr<TaskHub> task_hub{
            std::move(task_hub_builder).build(number_of_threads)
        };
        task_hub->schedule(root_task_handle.index());

        // TODO: use jthread when it is no longer experimental in libc++
        //       (or when it comes with no linker errors)
        std::vector<std::thread> worker_threads(number_of_threads - 1);
        for (auto index : std::views::iota(0u, worker_threads.size())) {
            worker_threads[index] = std::thread{
                [&should_stop, &task_hub, index] {
                    while (!should_stop.load()) {
                        task_hub->try_execute_a_generic_task(index + 1);
                    }
                },
            };
        }

        while (!should_stop.load()) {
            if (!task_hub->try_execute_a_main_only_task()) {
                task_hub->try_execute_a_generic_task(0);
            }
        }

        for (auto& worker_thread : worker_threads) {
            worker_thread.join();
        }
    }(AccessorProvidersTypeList{});
}
