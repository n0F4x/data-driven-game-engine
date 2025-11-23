module;

#include <memory>
#include <optional>
#include <utility>

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module ddge.modules.exec.v2.primitives.loop_until;

import ddge.modules.exec.v2.as_task_blueprint;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.v2.gatherers.WaitAll;
import ddge.modules.exec.v2.TaskBlueprint;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskBuilderBundle;
import ddge.modules.exec.v2.TaskBundle;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export template <convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T>
auto loop_until(
    MainTaskBlueprint_T&&                       main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>;

}   // namespace ddge::exec::v2

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilder<void>&& task_builder)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder);
}

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilderBundle<void>&& task_builder_bundle)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder_bundle).sync(ddge::exec::v2::WaitAllBuilder{});
}

template <ddge::exec::v2::convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T>
auto ddge::exec::v2::loop_until(
    MainTaskBlueprint_T&&                       main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>
{
    return TaskBlueprint<void, Cardinality::eSingle>{
        [x_main_blueprint      = as_task_blueprint<void>(std::move(main_blueprint)),
         x_predicate_blueprint = std::move(predicate_blueprint)]   //
        () mutable -> TaskBuilder<void> {
            return TaskBuilder<void>{
                [y_main_blueprint      = std::move(x_main_blueprint),
                 y_predicate_blueprint = std::move(x_predicate_blueprint)](
                    TaskHubBuilder&              task_hub_builder,
                    TaskFinishedCallback<void>&& callback
                ) mutable -> TaskBundle   //
                {
                    class Looper {
                    public:
                        auto set_task_predicate(TaskBundle&& predicate_task) -> void
                        {
                            m_predicate_task = std::move(predicate_task);
                        }
                        auto schedule_next_iteration(const TaskHubProxy& task_hub_proxy)
                            -> void
                        {
                            m_predicate_task->operator()(task_hub_proxy);
                        }

                    private:
                        std::optional<TaskBundle> m_predicate_task;
                    };

                    std::shared_ptr<Looper> looper{ std::make_shared<Looper>() };

                    TaskBundle predicate_task =
                        std::move(y_predicate_blueprint)
                            .materialize()
                            .build(
                                task_hub_builder,
                                [x_callback = std::move(callback),
                                 main_task =
                                     ::sync(std::move(y_main_blueprint).materialize())
                                         .build(
                                             task_hub_builder,
                                             [looper](const TaskHubProxy& task_hub_proxy) {
                                                 looper->schedule_next_iteration(
                                                     task_hub_proxy
                                                 );
                                             }
                                         )]                                   //
                                (const TaskHubProxy& task_hub_proxy,
                                 const bool          should_execute) mutable -> void   //
                                {
                                    if (should_execute) {
                                        main_task(task_hub_proxy);
                                    }
                                    else {
                                        x_callback(task_hub_proxy);
                                    }
                                }
                            );

                    looper->set_task_predicate(std::move(predicate_task));

                    return TaskBundle{
                        [looper](const TaskHubProxy& task_hub_proxy) {
                            looper->schedule_next_iteration(task_hub_proxy);
                        }   //
                    };
                }   //
            };
        }   //
    };
}
